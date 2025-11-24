#pragma once

#include "shape.h"
#include "thirdParty/glm/geometric.hpp"


struct Triangle : public Shape {

    Triangle(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2,
        const glm::vec3 &n0, const glm::vec3 &n1, const glm::vec3 &n2
        ) : p0(p0), p1(p1), p2(p2), n0(n0), n1(n1), n2(n2) {

    }

    Triangle(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2
        ) : p0(p0), p1(p1), p2(p2) {
        glm::vec3 e0 = p1 - p0;
        glm::vec3 e1 = p2 - p0;
        glm::vec3 normal = glm::normalize(glm::cross(e0, e1));

        n0 = normal;
        n1 = normal;
        n2 = normal;
    }


    std::optional<HitInfo> intersect(const Ray& ray, const float tmin, const float tmax) const override;

    glm::vec3 p0, p1, p2;
    glm::vec3 n0, n1, n2;

};
