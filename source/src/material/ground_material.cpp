#include "material/ground_material.h"
#include "sample/spherical.h"

glm::vec3 GroundMaterial::sampleBSDF(glm::vec3 const &hit_point, const glm::vec3 &view_direction, glm::vec3 &beta, const RNG &rng) const {
    beta *= albedo;

    // 0 8 16 24
    // [0,1) [8,9) [16,17) [24,25)
    // [0,0.125) [1,1.125) [2,2.125) [3,3.125) 边界长度0.125
    if( (static_cast<int>(glm::floor(hit_point.x * 8)) % 8 == 0) ||
        (static_cast<int>(glm::floor(hit_point.z * 8)) % 8 == 0) ){
        beta *= 0.1;
    }

    return CosineSampleHemisphere({rng.uniform(),rng.uniform()});
}