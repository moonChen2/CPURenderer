//
// Created by hy on 2025/12/10.
//

#include "material/specular_material.h"


glm::vec3 SpecularMaterial::sampleBRDF(glm::vec3 const &view_direction, glm::vec3 &beta, RNG const &rng) const {
    // brdf = hit_info->material->albedo / light_direction.y;
    // pdf = 1;
    // 局部坐标系下
    // glm::dot(light_direction, glm::vec3(0, 1, 0)) = light_direction.y
    // beta *= brdf * light_direction.y / pdf;
    // beta *= hit_info->material->albedo;
    beta *= albedo;

    return glm::vec3 {-view_direction.x, view_direction.y, -view_direction.z};
}
