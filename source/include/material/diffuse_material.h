#pragma once

#include "material.h"


class DiffuseMaterial : public Material {
public:
    DiffuseMaterial(glm::vec3 const &albedo) : albedo(albedo) {}
    std::optional<BSDFSample> sampleBSDF(glm::vec3 const &hit_point, glm::vec3 const &view_direction, RNG const &rng) const override;

private:
    glm::vec3 albedo {};
};