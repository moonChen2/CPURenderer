#include "accelerate/bvh.h"


void BVH::build(std::vector<Triangle> &&triangles) {
    BVHTreeNode *root = new BVHTreeNode {};
    root->triangles = std::move(triangles);
    root->updateBounds();
    root->depth = 1;
    recursiveSplit(root);
    recursiveFlatten(root);
}

void BVH::recursiveSplit(BVHTreeNode *node) {
    if (node->triangles.size() == 1) return;

    auto diag = node->bounds.diagonal();

    size_t max_axis = diag.x > diag.y ? (diag.x > diag.z ? 0 : 2) : (diag.y > diag.z ? 1 : 2);
    float mid = node->bounds.b_min[max_axis] + diag[max_axis] * 0.5f;

    std::vector<Triangle> child0_triangles, child1_triangles;
    for (const auto &triangle : node->triangles) {
        if (triangle.p0[max_axis] + triangle.p1[max_axis] + triangle.p2[max_axis] < mid * 3.f) {
            child0_triangles.push_back(triangle);
        }else {
            child1_triangles.push_back(triangle);
        }
    }

    if (child0_triangles.empty() || child1_triangles.empty()) {
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

    recursiveSplit(child0);
    recursiveSplit(child1);
}
//返回当前node放进去的下标
size_t BVH::recursiveFlatten(BVHTreeNode *node) {
    BVHNode bvh_node {
        node->bounds,
        0,
        static_cast<uint16_t>(node->triangles.size()),
        static_cast<uint8_t>(node->depth)
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

    std::array<int, 32> stack;
    //将要遍历的节点下标
    auto ptr = stack.begin();
    //当前遍历的节点下标
    size_t current_node_index = 0;

    while (true) {
        auto &node = nodes[current_node_index];

        if (!node.bounds.hasIntersection(ray, t_min, t_max)) {
            if (ptr == stack.begin()) {
                break;
            }
            current_node_index = *(--ptr);
            continue;
        }
        //非叶子节点，没有三角形
        if (node.triangle_count == 0) {
            //非叶子节点的下一个节点一定是他的左孩子
            current_node_index++;
            //右子树压栈供稍后访问
            *(ptr++) = node.child1_index;
        } else {
            auto triangle_iter = ordered_triangles.begin() + node.triangle_index;
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

    return closest_hit_Info;
}


