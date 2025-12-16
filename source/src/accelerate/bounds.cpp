#include "accelerate/bounds.h"

bool Bounds::hasIntersection(const Ray &ray, float t_min, float t_max) const{
    //ray 和 bmin bmax交点
    glm::vec3 t1 = (b_min - ray.origin) / ray.direction;
    glm::vec3 t2 = (b_max - ray.origin) / ray.direction;
    //三个轴的进入时间
    glm::vec3 tmin = glm::min(t1, t2);
    //三个轴的离开时间
    glm::vec3 tmax = glm::max(t1, t2);

    //最晚进入时间
    float near = glm::max(tmin.x, glm::max(tmin.y, tmin.z));
    //最早离开时间
    float far = glm::min(tmax.x, glm::min(tmax.y, tmax.z));

    // Lecture 15 Fixed
    //if (near <= t_min && t_max <= far) return false;

    near = glm::max(near, t_min);
    far = glm::min(far, t_max);

    return near <= far;
}

bool Bounds::hasIntersection(const Ray &ray, const glm::vec3 &inv_direction, float t_min, float t_max) const {
    //ray 和 bmin bmax交点
    glm::vec3 t1 = (b_min - ray.origin) * inv_direction;
    glm::vec3 t2 = (b_max - ray.origin) * inv_direction;
    //三个轴的进入时间
    glm::vec3 tmin = glm::min(t1, t2);
    //三个轴的离开时间
    glm::vec3 tmax = glm::max(t1, t2);

    //最晚进入时间
    float near = glm::max(tmin.x, glm::max(tmin.y, tmin.z));
    //最早离开时间
    float far = glm::min(tmax.x, glm::min(tmax.y, tmax.z));

    // Lecture 15 Fixed
    //if (near <= t_min && t_max <= far) return false;

    near = glm::max(near, t_min);
    far = glm::min(far, t_max);

    return near <= far;
}
