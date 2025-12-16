//
// Created by hy on 2025/12/10.
//

#include "material/specular_material.h"


std::optional<BSDFSample> SpecularMaterial::sampleBSDF(glm::vec3 const &hit_point, glm::vec3 const &view_direction, RNG const &rng) const {
    // brdf = hit_info->material->albedo / light_direction.y;
    // pdf = 1;
    // 局部坐标系下
    // glm::dot(light_direction, glm::vec3(0, 1, 0)) = light_direction.y
    // beta *= brdf * light_direction.y / pdf;
    // beta *= hit_info->material->albedo;
    glm::vec3 light_direction {-view_direction.x, view_direction.y, -view_direction.z};
    glm::vec3 bsdf = albedo / glm::abs(light_direction.y);
    float pdf = 1;
    return BSDFSample { bsdf, pdf, light_direction};
}
