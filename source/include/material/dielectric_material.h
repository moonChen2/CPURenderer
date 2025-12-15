#pragma once

#include "material.h"

class DielectricMaterial : public Material {
public:
    DielectricMaterial(float ior, glm::vec3 const &albedo)
        : ior(ior), albedo_r(albedo), albedo_t(albedo) {}

    DielectricMaterial(float ior, glm::vec3 const &albedo_r, glm::vec3 const &albedo_t)
            : ior(ior), albedo_r(albedo_r), albedo_t(albedo_t) {}

    glm::vec3 sampleBSDF(glm::vec3 const &hit_point, const glm::vec3 &view_direction, glm::vec3 &beta, const RNG &rng) const override;
private:
    float ior;
    glm::vec3 albedo_r, albedo_t;
};
