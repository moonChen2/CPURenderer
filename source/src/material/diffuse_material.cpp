#include "material/diffuse_material.h"
#include "sample/spherical.h"

std::optional<BSDFSample> DiffuseMaterial::sampleBSDF(glm::vec3 const &hit_point, glm::vec3 const &view_direction, RNG const &rng) const {
    // pdf = 1 / 2pi;
    // brdf = hit_info->material->albedo / pi;
    // beta *= brdf * light_direction.y / pdf;
    // beta *= hit_info->material->albedo * light_direction.y * 2.f;

    // Cosine Weight
    // pdf = light_direction.y / pi
    // brdf = hit_info->material->albedo / pi
    // light_direction = CosineSampleHemisphere( {rng.uniform(), rng.uniform()});
    // beta *= hit_info->material->albedo;

    glm::vec3 light_direction = CosineSampleHemisphere({rng.uniform(),rng.uniform()});
    glm::vec3 bsdf = albedo ;
    float pdf = light_direction.y;
    return BSDFSample { bsdf, pdf, light_direction};
}
