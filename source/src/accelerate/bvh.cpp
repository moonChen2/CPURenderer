#include "accelerate/bvh.h"
#include "util/debug_marco.h"
#include <iostream>
void BVH::build(std::vector<Triangle> &&triangles) {
    BVHTreeNode *root = new BVHTreeNode {};
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
    std::vector<Triangle> child0_triangles, child1_triangles;
    for(size_t axis = 0; axis < 3; axis++){
        // SAH
        for(size_t i = 0; i < 11; i++){
            float mid = node->bounds.b_min[axis] + diag[axis] * (i + 1.f) / 12.f;

            Bounds child0_bounds{}, child1_bounds{};
            std::vector<Triangle> child0_triangles_temp, child1_triangles_temp;
            for (const auto &triangle : node->triangles) {
                if (triangle.p0[axis] + triangle.p1[axis] + triangle.p2[axis] < mid * 3.f) {
                    child0_bounds.expend(triangle.p0);
                    child0_bounds.expend(triangle.p1);
                    child0_bounds.expend(triangle.p2);
                    child0_triangles_temp.push_back(triangle);
                }else {
                    child1_bounds.expend(triangle.p0);
                    child1_bounds.expend(triangle.p1);
                    child1_bounds.expend(triangle.p2);
                    child1_triangles_temp.push_back(triangle);
                }
            }

            if(child0_triangles_temp.empty() || child1_triangles_temp.empty()){
                continue;
            }

            float cost = child0_bounds.area() * child0_triangles_temp.size() + child1_bounds.area() * child1_triangles_temp.size();
            if(cost < min_cost){
                min_cost = cost;
                child0_triangles = std::move(child0_triangles_temp);
                child1_triangles = std::move(child1_triangles_temp);
                node->split_axis = axis;
            }
        }
    }

    if (child0_triangles.empty() || child1_triangles.empty()) {
        state.addLeafNode(node);
        return ;
    }

    auto *child0 = new BVHTreeNode {};
    auto *child1 = new BVHTreeNode {};
    node->children[0] = child0;
    node->children[1] = child1;
    node->triangles.clear();
    node->triangles.shrink_to_fit();
    child0->depth = node->depth + 1;
    child1->depth = node->depth + 1;
    child0->triangles = std::move(child0_triangles);
    child1->triangles = std::move(child1_triangles);
    child0->updateBounds();
    child1->updateBounds();

    recursiveSplit(child0, state);
    recursiveSplit(child1, state);
}
//返回当前node放进去的下标
size_t BVH::recursiveFlatten(BVHTreeNode *node) {
    BVHNode bvh_node {
        node->bounds,
        0,
        static_cast<uint16_t>(node->triangles.size()),
        static_cast<uint8_t>(node->depth),
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

                    DEBUG_LINE(closest_hit_Info->bounds_depth = node.depth)
                }
            }
            if (ptr == stack.begin()) {
                break;
            }
            current_node_index = *(--ptr);
        }
    }

    if(closest_hit_Info.has_value()){
        DEBUG_LINE(closest_hit_Info->bounds_test_count = bounds_test_count)
        DEBUG_LINE(closest_hit_Info->triangle_test_count = triangle_test_count)
    }

    return closest_hit_Info;
}


