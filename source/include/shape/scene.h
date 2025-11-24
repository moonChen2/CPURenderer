#pragma once
#include "shape.h"
#include <thirdParty/glm/glm.hpp>
#include <thirdParty/glm/ext/matrix_transform.hpp>


struct ShapeInstance {
    const Shape &shape;
    Material material;
    glm::mat4 world_from_object;
    glm::mat4 object_from_world;
};

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
private:
    std::vector<ShapeInstance> instances;
};
