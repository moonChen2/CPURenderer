#pragma once
#include "shape.h"
#include <thirdParty/glm/glm.hpp>
#include <thirdParty/glm/ext/matrix_transform.hpp>
#include "accelerate/scene_bvh.h"


struct Scene : public Shape{
public:
    void addShape(const Shape &shape,
        Material const *material = nullptr,
        glm::vec3 const &pos = {0.0f, 0.0f, 0.0f},
        glm::vec3 const &scale = {1.0f, 1.0f, 1.0f},
        glm::vec3 const &rotate = {0.0f, 0.0f, 0.0f});

    std::optional<HitInfo> intersect(Ray const &ray,
        float t_min = 1e-5,
        float t_max = std::numeric_limits<float>::infinity()) const override;

    void build() { scene_bvh.build(std::move(instances)); }
private:
    std::vector<ShapeInstance> instances;
    SceneBVH scene_bvh {};
};
