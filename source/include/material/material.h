#pragma once

#include "thirdParty/glm/glm.hpp"
#include "util/rng.h"

struct BSDFSample {
    glm::vec3 bsdf;
    float pdf;
    glm::vec3 light_direction;
};

class Material {
public:
    virtual std::optional<BSDFSample> sampleBSDF(glm::vec3 const &hit_point, glm::vec3 const &view_direction, RNG const &rng) const = 0;
    void setEmissive(glm::vec3 const &emissive){ this->emissive = emissive; }

public:
    glm::vec3 emissive;

};
