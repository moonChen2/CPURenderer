#pragma once
#include "material.h"

class GroundMaterial : public Material{
public:
    GroundMaterial(glm::vec3 const &albedo) : albedo(albedo){}

    std::optional<BSDFSample> sampleBSDF(glm::vec3 const &hit_point, const glm::vec3 &view_direction, const RNG &rng) const override;
private:
    glm::vec3 albedo;
};