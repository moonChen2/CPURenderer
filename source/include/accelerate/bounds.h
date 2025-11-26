#pragma once
#include "camera/ray.h"
#include "thirdParty/glm/glm.hpp"



struct Bounds {

    Bounds() : b_min(0.0), b_max(0.0) {}

    Bounds(const glm::vec3 &b_min, const glm::vec3 &b_max) : b_min(b_min), b_max(b_max) {}

    void expend(const glm::vec3 &pos) {
        b_min = glm::min(b_min, pos);
        b_max = glm::max(b_max, pos);
    }

    bool hasIntersection(const Ray &ray, float t_min, float t_max) const;

    glm::vec3 b_min;
    glm::vec3 b_max;

};
