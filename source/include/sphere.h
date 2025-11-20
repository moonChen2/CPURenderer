#pragma once

#include "ray.h"
#include <optional>

struct Sphere {
    glm::vec3 center;
    float radius;

    std::optional<float> intersect(const Ray &ray) const;
};