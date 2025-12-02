#pragma once
#include "shape.h"
#include <thirdParty/glm/glm.hpp>
#include <thirdParty/glm/ext/matrix_transform.hpp>
#include "accelerate/scene_bvh.h"


struct Scene : public Shape{
public:
    void addShape(const Shape &shape,
        const Material &material = {},
        const glm::vec3 &pos = {0.0f, 0.0f, 0.0f},
        const glm::vec3 &scale = {1.0f, 1.0f, 1.0f},
        const glm::vec3 &rot = {0.0f, 0.0f, 0.0f});

    std::optional<HitInfo> intersect(const Ray &ray,
        float t_min = 1e-5,
        float t_max = std::numeric_limits<float>::infinity()) const override;

    void build() { scene_bvh.build(std::move(instances)); }
private:
    std::vector<ShapeInstance> instances;
    SceneBVH scene_bvh {};
};
