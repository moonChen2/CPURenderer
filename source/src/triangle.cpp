#include "triangle.h"


std::optional<HitInfo> Triangle::intersect(const Ray &ray, const float tmin, const float tmax) const {
    glm::vec3 e1 = p1 - p0;
    glm::vec3 e2 = p2 - p0;
    glm::vec3 s = ray.origin - p0;

    glm::vec3 s1 = glm::cross(ray.direction, e2);
    glm::vec3 s2 = glm::cross(s, e1);

    float inv_det = 1.f / glm::dot(s1, e1);

    float u = inv_det * glm::dot(s1, s);
    if (u < 0 || u > 1) return {};

    float v = inv_det * glm::dot(s2, ray.direction);
    if (v < 0 || u + v > 1) return {};

    float hit_t = inv_det * glm::dot(s2, e2);
    if (hit_t > tmin && hit_t < tmax) {
        glm::vec3 hit_point = ray.hit(hit_t);
        glm::vec3 normal = (1.f - u - v) * n0 + u * n1 + v * n2;
        return HitInfo{hit_t, hit_point, normal};
    }

    return {};
}
