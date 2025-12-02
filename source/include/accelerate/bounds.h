#pragma once
#include "camera/ray.h"
#include "thirdParty/glm/glm.hpp"



struct Bounds {

    Bounds() : b_min(std::numeric_limits<float>::infinity()), b_max(-std::numeric_limits<float>::infinity()) {}

    Bounds(const glm::vec3 &b_min, const glm::vec3 &b_max) : b_min(b_min), b_max(b_max) {}

    void expend(const glm::vec3 &pos) {
        b_min = glm::min(b_min, pos);
        b_max = glm::max(b_max, pos);
    }
    void expend(const Bounds &bounds) {
        b_min = glm::min(b_min, bounds.b_min);
        b_max = glm::max(b_max, bounds.b_max);
    }
    bool hasIntersection(const Ray &ray, float t_min, float t_max) const;

    bool hasIntersection(const Ray &ray, const glm::vec3 &inv_direction, float t_min, float t_max) const;

    glm::vec3 diagonal() const {
        return b_max - b_min;
    }

    float area() const{
        auto diag = diagonal();

        return 2.f * (diag.x * diag.y + diag.y * diag.z + diag.x * diag.z);
    }

    bool isValid() const {
        return b_max.x >= b_min.x && b_max.y >= b_min.y && b_max.z >= b_min.z;
    }

    //好帅的位运算
    // 0	000	(b_min.x, b_min.y, b_min.z)	左下后（Left-Bottom-Back）
    // 1	001	(b_max.x, b_min.y, b_min.z)	右下后
    // 2	010	(b_min.x, b_max.y, b_min.z)	左上后
    // 3	011	(b_max.x, b_max.y, b_min.z)	右上后
    // 4	100	(b_min.x, b_min.y, b_max.z)	左下前
    // 5	101	(b_max.x, b_min.y, b_max.z)	右下前
    // 6	110	(b_min.x, b_max.y, b_max.z)	左上前
    // 7	111	(b_max.x, b_max.y, b_max.z)	右上前（默认初始值）
    glm::vec3 getCorner(size_t idx) const {
        auto corner = b_max;
        if ((idx & 0b1) == 0) corner.x = b_min.x;
        if ((idx & 0b10) == 0) corner.y = b_min.y;
        if ((idx & 0b100) == 0) corner.z = b_min.z;
        return corner;
    }

    glm::vec3 b_min;
    glm::vec3 b_max;

};
