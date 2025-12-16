#pragma once

#include "material.h"

class SpecularMaterial : public Material {
public:
    SpecularMaterial(glm::vec3 const &albedo) : albedo(albedo) {}
    std::optional<BSDFSample> sampleBSDF(glm::vec3 const &hit_point, glm::vec3 const &view_direction, RNG const &rng) const override;

private:
    glm::vec3 albedo {};
};

