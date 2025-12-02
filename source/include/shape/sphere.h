#pragma once

#include "shape.h"

struct Sphere : public Shape{

    Sphere(glm::vec3 center, float radius) : center(center), radius(radius) {};

    glm::vec3 center;
    float radius;

    std::optional<HitInfo> intersect(const Ray &ray, float t_min, float t_max) const override;

    Bounds getBounds() const override {
        return {center - radius, center + radius};
    }
};