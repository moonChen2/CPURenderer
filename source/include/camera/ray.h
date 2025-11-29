#pragma once
#include "thirdParty/glm/glm.hpp"
#include "shape/material.h"
#include "util/debug_marco.h"
struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;

    glm::vec3 hit(float t) const{
        return origin + direction * t;
    }

    Ray objectFromWorld(const glm::mat4 &object_from_world) const;
};


struct HitInfo {
    float t;
    glm::vec3 hit_point;
    glm::vec3 normal;
    const Material *material = nullptr;

    DEBUG_LINE(size_t bounds_test_count = 0)
    DEBUG_LINE(size_t triangle_test_count = 0)
    DEBUG_LINE(size_t bounds_depth = 0)

};
