#pragma once
#include <vector>

#include "bounds.h"
#include "shape/triangle.h"


struct BVHTreeNode {
    Bounds bounds;
    std::vector<Triangle> triangles;
    BVHTreeNode *children[2];

    size_t depth;
    size_t split_axis;

    void updateBounds() {
        for (const auto &triangle : triangles) {
            bounds.expend(triangle.p0);
            bounds.expend(triangle.p1);
            bounds.expend(triangle.p2);
        }
    }
};

//Flatten vector
struct alignas(32) BVHNode {
    Bounds bounds;              // 6 * 4
    union {
        int child1_index;
        int triangle_index;
    };                          // 4
    uint16_t triangle_count;    // 2
    uint8_t depth;              // 1
    uint8_t split_axis;         // 1
};

struct BVHState{
    size_t total_node_count {};
    size_t leaf_node_count {};
    size_t max_leaf_node_triangle_count {};

    void addLeafNode(BVHTreeNode *node){
        leaf_node_count++;
        max_leaf_node_triangle_count = glm::max(max_leaf_node_triangle_count, node->triangles.size());
    }
};

class BVH : public Shape{
public:
    void build(std::vector<Triangle> &&triangles);
    std::optional<HitInfo> intersect(const Ray &ray, float t_min, float t_max) const override;
private:
    void recursiveSplit(BVHTreeNode *node, BVHState &state);
    size_t recursiveFlatten(BVHTreeNode *node);
private:
    std::vector<BVHNode > nodes;
    std::vector<Triangle> ordered_triangles;
};