#include "renderer/normal_renderer.h"


glm::vec3 NormalRenderer::renderPixel(const glm::ivec3 &pixel_coord) {
    auto ray = camera.generateRay(pixel_coord);

    auto hit_info = scene.intersect(ray);

    if (hit_info.has_value()) {
        //-1,1 -> 0,1
        return hit_info->normal * 0.5f + 0.5f;
    }

    return {};
}
