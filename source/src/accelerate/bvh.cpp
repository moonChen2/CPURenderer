#include "accelerate/bvh.h"
#include "util/debug_marco.h"
#include <iostream>
void BVH::build(std::vector<Triangle> &&triangles) {
    BVHTreeNode *root = allocator.allocate();
    root->triangles = std::move(triangles);
    root->updateBounds();
    root->depth = 1;

    BVHState state {};
    size_t triangle_count = root->triangles.size();
    recursiveSplit(root, state);

    std::cout << "Total Node Count: " << state.total_node_count << std::endl;
    std::cout << "Leaf Node Count: " << state.leaf_node_count << std::endl;
    std::cout << "Triangle Count: " << triangle_count << std::endl;
    std::cout << "Mean Leaf Node Triangle Count: " << static_cast<float>(triangle_count) / static_cast<float>(state.leaf_node_count) << std::endl;
    std::cout << "Max Leaf Node Triangle Count: " << state.max_leaf_node_triangle_count << std::endl;
    std::cout << "Max Leaf Node Depth: " << state.max_leaf_node_depth << std::endl;

    //提前分配空间
    nodes.reserve(state.total_node_count);
    ordered_triangles.reserve(triangle_count);
    recursiveFlatten(root);
}

void BVH::recursiveSplit(BVHTreeNode *node, BVHState &state) {
    state.total_node_count ++;
    //最多32层
    if (node->triangles.size() == 1 || node->depth > 32){
        state.addLeafNode(node);
        return;
    }

    auto diag = node->bounds.diagonal();
    float min_cost = std::numeric_limits<float>::infinity();
    size_t min_split_index = 0;                                 //bucket index
    //cost最小的时候的包围盒
    Bounds min_child0_bounds{}, min_child1_bounds{};
    //cost最小的时候两个子节点三角形数量
    size_t min_child0_triangle_count = 0, min_child1_triangle_count = 0;
    constexpr size_t bucket_count = 12;
    std::vector<size_t> triangle_indices_buckets[3][bucket_count] = {};
    for(size_t axis = 0; axis < 3; axis++){
        // SAH
        // bucket speed up
        Bounds bounds_buckets[bucket_count] = {};
        size_t triangle_count_buckets[bucket_count] = {};
        size_t triangle_idx = 0;

        for(const auto &triangle : node->triangles){
            auto triangle_center = (triangle.p0[axis] + triangle.p1[axis] + triangle.p2[axis] ) / 3.f;
            size_t bucket_idx = glm::clamp<size_t>(
                    glm::floor((triangle_center - node->bounds.b_min[axis]) * bucket_count / diag[axis]),
                    0, bucket_count - 1 );

            bounds_buckets[bucket_idx].expend(triangle.p0);
            bounds_buckets[bucket_idx].expend(triangle.p1);
            bounds_buckets[bucket_idx].expend(triangle.p2);
            triangle_count_buckets[bucket_idx] ++;
            triangle_indices_buckets[axis][bucket_idx].push_back(triangle_idx);
            triangle_idx++;
        }

        Bounds left_bounds = bounds_buckets[0];
        size_t left_triangle_count = triangle_count_buckets[0];
        for(size_t i = 1; i <= bucket_count - 1; i++){
            Bounds right_bounds {};
            size_t right_triangle_count = 0;
            for(size_t j = bucket_count - 1; j>=i; j--){
                right_bounds.expend(bounds_buckets[j]);
                right_triangle_count += triangle_count_buckets[j];
            }
            if(right_triangle_count == 0){
                break;
            }
            if(left_triangle_count != 0){
                float cost = left_triangle_count * left_bounds.area() + right_triangle_count * right_bounds.area();
                if(cost < min_cost){
                    min_cost = cost;
                    node->split_axis = axis;
                    min_split_index = i;
                    min_child0_bounds = left_bounds;
                    min_child1_bounds = right_bounds;
                    min_child0_triangle_count = left_triangle_count;
                    min_child1_triangle_count = right_triangle_count;
                }
            }
            left_bounds.expend(bounds_buckets[i]);
            left_triangle_count += triangle_count_buckets[i];
        }

    }

    if(min_split_index == 0) {
        state.addLeafNode(node);
        return;
    }

    auto *child0 = allocator.allocate();
    auto *child1 = allocator.allocate();
    node->children[0] = child0;
    node->children[1] = child1;

    child0->triangles.reserve(min_child0_triangle_count);
    child1->triangles.reserve(min_child1_triangle_count);
    for(size_t i = 0; i < min_split_index; i++){
        for(size_t idx : triangle_indices_buckets[node->split_axis][i]){
            child0->triangles.push_back(node->triangles[idx]);
        }
    }
    for(size_t i = min_split_index; i < bucket_count; i++){
        for(size_t idx : triangle_indices_buckets[node->split_axis][i]){
            child1->triangles.push_back(node->triangles[idx]);
        }
    }
    node->triangles.clear();
    node->triangles.shrink_to_fit();
    child0->depth = node->depth + 1;
    child1->depth = node->depth + 1;
    child0->bounds = min_child0_bounds;
    child1->bounds = min_child1_bounds;

    recursiveSplit(child0, state);
    recursiveSplit(child1, state);
}
//返回当前node放进去的下标
size_t BVH::recursiveFlatten(BVHTreeNode *node) {
    BVHNode bvh_node {
        node->bounds,
        0,
        static_cast<uint16_t>(node->triangles.size()),
        static_cast<uint8_t>(node->split_axis)
    };

    auto idx = nodes.size();
    nodes.push_back(bvh_node);
    if (bvh_node.triangle_count == 0) {
        recursiveFlatten(node->children[0]);
        nodes[idx].child1_index = recursiveFlatten(node->children[1]);
    } else {
        nodes[idx].triangle_index = ordered_triangles.size();
        for (const auto &triangle: node->triangles) {
            ordered_triangles.push_back(triangle);
        }
    }
    return idx;
}


std::optional<HitInfo> BVH::intersect(const Ray &ray, float t_min, float t_max) const {
    std::optional<HitInfo> closest_hit_Info;

    DEBUG_LINE(size_t bounds_test_count = 0, triangle_test_count = 0)

    glm::bvec3 dir_is_neg = {
            ray.direction.x < 0,
            ray.direction.y < 0,
            ray.direction.z < 0,
    };
    glm::vec3 inv_direction = 1.f / ray.direction;

    std::array<int, 32> stack;
    //将要遍历的节点下标
    auto ptr = stack.begin();
    //当前遍历的节点下标
    size_t current_node_index = 0;

    while (true) {
        auto &node = nodes[current_node_index];

        DEBUG_LINE(bounds_test_count++)

        if (!node.bounds.hasIntersection(ray, t_min, t_max)) {
            if (ptr == stack.begin()) {
                break;
            }
            current_node_index = *(--ptr);
            continue;
        }
        //非叶子节点，没有三角形
        if (node.triangle_count == 0) {
            //以x轴划分为例子，如果从左到右射入，应该先遍历左孩子，反之应该先遍历右孩子
            if(dir_is_neg[node.split_axis]){
                *(ptr++) = current_node_index + 1;
                current_node_index = node.child1_index;
            }else{
                //非叶子节点的下一个节点一定是他的左孩子
                current_node_index++;
                //右子树压栈供稍后访问
                *(ptr++) = node.child1_index;
            }
        } else {
            auto triangle_iter = ordered_triangles.begin() + node.triangle_index;

            DEBUG_LINE(triangle_test_count += node.triangle_count)

            for (size_t i = 0; i < node.triangle_count; i++) {
                auto hit_info = triangle_iter->intersect(ray, t_min, t_max);
                ++triangle_iter;
                if (hit_info) {
                    t_max = hit_info->t;
                    closest_hit_Info = hit_info;
                }
            }
            if (ptr == stack.begin()) {
                break;
            }
            current_node_index = *(--ptr);
        }
    }


    DEBUG_LINE(ray.bounds_test_count += bounds_test_count)
    DEBUG_LINE(ray.triangle_test_count += triangle_test_count)


    return closest_hit_Info;
}


