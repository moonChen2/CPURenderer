#pragma once

#include "camera/ray.h"
#include <optional>

struct Shape {
    virtual std::optional<HitInfo> intersect(
        const Ray &ray,float t_min,float t_max = std::numeric_limits<float>::infinity()) const = 0;
};