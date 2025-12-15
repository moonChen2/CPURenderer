#pragma once

#include "thirdParty/glm/glm.hpp"
#include "util/rng.h"

class Material {
public:
    virtual glm::vec3 sampleBSDF(glm::vec3 const &hit_point, glm::vec3 const &view_direction, glm::vec3 &beta, RNG const &rng) const = 0;
    void setEmissive(glm::vec3 const &emissive){ this->emissive = emissive; }

public:
    glm::vec3 emissive;

};
