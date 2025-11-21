#include "scene.h"

void Scene::addShape(const Shape *shape, const glm::vec3 &pos, const glm::vec3 &rot, const glm::vec3 &scale) {
    glm::mat4 world_from_object = glm::translate(glm::mat4(1.f), pos) *
        glm::rotate(glm::mat4(1.f), glm::radians(rot.x), {1.f, 0.f, 0.f}) *
        glm::rotate(glm::mat4(1.f), glm::radians(rot.y), glm::vec3{0.f, 1.f, 0.f}) *
        glm::rotate(glm::mat4(1.f), glm::radians(rot.z), {0.f, 0.f, 1.f}) *
        glm::scale(glm::mat4(1.f), scale) ;

    instances.push_back(ShapeInstance(shape, world_from_object, glm::inverse(world_from_object)));
}


std::optional<HitInfo> Scene::intersect(const Ray &ray, float t_min, float t_max) const {
    std::optional<HitInfo> closest_hit_info;
    const ShapeInstance *closest_instance = nullptr;

    for (const auto &instance : instances) {
        auto ray_object = ray.objectFromWorld(instance.object_from_world);

        auto hit_info = instance.shape->intersect(ray_object, t_min, t_max);
        if (hit_info.has_value()) {
            closest_hit_info = hit_info;
            t_max = hit_info->t;
            closest_instance = &instance;
        }

    }

    if (closest_instance) {
        closest_hit_info->hit_point =
            closest_instance->world_from_object * glm::vec4(closest_hit_info->hit_point, 1.f);
        closest_hit_info->normal =
            glm::transpose(closest_instance->object_from_world) * glm::vec4(closest_hit_info->normal, 0.f);
        closest_hit_info->normal = glm::normalize(closest_hit_info->normal);
    }

    return closest_hit_info;
}
