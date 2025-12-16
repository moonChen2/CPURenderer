#pragma once

#include "material.h"


class ConductorMaterial : public Material{
public:
    ConductorMaterial(glm::vec3 const &ior, glm::vec3 const &k) : ior(ior), k(k){}

    std::optional<BSDFSample> sampleBSDF(const glm::vec3 &hit_point, const glm::vec3 &view_direction, const RNG &rng) const override;
private:
    glm::vec3 ior, k;
};
