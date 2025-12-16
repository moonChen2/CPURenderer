#include "material/ground_material.h"
#include "sample/spherical.h"

std::optional<BSDFSample> GroundMaterial::sampleBSDF(glm::vec3 const &hit_point, const glm::vec3 &view_direction, const RNG &rng) const {
    glm::vec3 light_direction = CosineSampleHemisphere({rng.uniform(),rng.uniform()});
    glm::vec3 bsdf = albedo ;
    float pdf = light_direction.y;

    // 0 8 16 24
    // [0,1) [8,9) [16,17) [24,25)
    // [0,0.125) [1,1.125) [2,2.125) [3,3.125) 边界长度0.125
    if( (static_cast<int>(glm::floor(hit_point.x * 8)) % 8 == 0) ||
        (static_cast<int>(glm::floor(hit_point.z * 8)) % 8 == 0) ){
        bsdf *= 0.1;
    }

    return BSDFSample { bsdf, pdf, light_direction};
}