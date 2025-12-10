#include "shape/scene.h"

void Scene::addShape(const Shape &shape, Material const *material, glm::vec3 const &pos, glm::vec3 const &scale, glm::vec3 const &rotate) {
    glm::mat4 world_from_object =
            glm::translate(glm::mat4(1.f), pos) *
            glm::rotate(glm::mat4(1.f), glm::radians(rotate.z), { 0, 0, 1 }) *
            glm::rotate(glm::mat4(1.f), glm::radians(rotate.y), { 0, 1, 0 }) *
            glm::rotate(glm::mat4(1.f), glm::radians(rotate.x), { 1, 0, 0 }) *
            glm::scale(glm::mat4(1.f), scale);
    instances.push_back(ShapeInstance {
        .shape = shape,
        .material = material,
        .world_from_object = world_from_object,
        .object_from_world = glm::inverse(world_from_object)
    });
}


std::optional<HitInfo> Scene::intersect(const Ray &ray, float t_min, float t_max) const {
    return scene_bvh.intersect(ray, t_min, t_max);
}
