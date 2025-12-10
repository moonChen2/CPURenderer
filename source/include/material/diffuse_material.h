#pragma once

#include "material.h"


class DiffuseMaterial : public Material {
public:
    DiffuseMaterial(glm::vec3 const &albedo) : albedo(albedo) {}
    glm::vec3 sampleBRDF(glm::vec3 const &view_direction, glm::vec3 &beta, RNG const &rng) const override;

private:
    glm::vec3 albedo {};
};