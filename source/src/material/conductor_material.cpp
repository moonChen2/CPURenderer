#include "material/conductor_material.h"
#include "util/complex.h"

std::optional<BSDFSample> ConductorMaterial::sampleBSDF(const glm::vec3 &hit_point, const glm::vec3 &view_direction, const RNG &rng) const {
    glm::vec3 fr {};
    for (size_t i = 0; i < 3; i ++) {
        Complex etat_div_etai { ior[i], k[i] };
        float cos_theta_i = glm::clamp(view_direction.y, 0.f, 1.f);
        float sin2_theta_i = 1.f - cos_theta_i * cos_theta_i;
        Complex sin2_theta_t = sin2_theta_i / (etat_div_etai * etat_div_etai);
        Complex cos_theta_t = sqrt(1.f - sin2_theta_t);

        Complex r_parl = (etat_div_etai * cos_theta_i - cos_theta_t) / (etat_div_etai * cos_theta_i + cos_theta_t);
        Complex r_perp = (cos_theta_i - etat_div_etai * cos_theta_t) / (cos_theta_i + etat_div_etai * cos_theta_t);

        fr[i] = 0.5 * (norm(r_parl) + norm(r_perp));
    }

    //导体只会发生镜面反射
    glm::vec3 light_direction {-view_direction.x, view_direction.y, -view_direction.z };
    //吸收一部分
    glm::vec3 bsdf = fr / glm::abs(light_direction.y) ;
    float pdf = 1;


    return BSDFSample{ bsdf, pdf, light_direction };
}