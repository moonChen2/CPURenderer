#include "material/dielectric_material.h"

//etai_div_etat = ni / nt
float Fresnel(float etai_div_etat, float cos_theta_t, float &cos_theta_i){
    float sin2_theta_t = 1 - cos_theta_t * cos_theta_t;
    float sin2_theta_i = sin2_theta_t / (etai_div_etat * etai_div_etat);

    if (sin2_theta_i >= 1) {
        return 1;
    }

    cos_theta_i = glm::sqrt(1 - sin2_theta_i);
    float r_parl = (cos_theta_i - etai_div_etat * cos_theta_t) / (cos_theta_i + etai_div_etat * cos_theta_t);
    float r_perp = (etai_div_etat * cos_theta_i - cos_theta_t) / (etai_div_etat * cos_theta_i + cos_theta_t);
    return 0.5 * (r_parl * r_parl + r_perp * r_perp);
}

glm::vec3 DielectricMaterial::sampleBSDF(glm::vec3 const &hit_point, const glm::vec3 &view_direction, glm::vec3 &beta, const RNG &rng) const {
    //etai_div_etat = ni / nt
    float etai_div_etat = ior;
    glm::vec3 normal = {0,1,0};
    float cos_theta_t = view_direction.y;
    if(cos_theta_t < 0){
        etai_div_etat = 1.f / ior;
        normal = {0,-1,0};
        cos_theta_t = -cos_theta_t;
    }

    float cos_theta_i;
    float fr = Fresnel(etai_div_etat, cos_theta_t, cos_theta_i);

    // 反射
    if(rng.uniform() <= fr){
        beta *= albedo_r;
        return glm::vec3 { -view_direction.x, view_direction.y, -view_direction.z};
    }else{
        // 折射
        // btdf非对称性导致有个分母
        beta *= albedo_t / (etai_div_etat * etai_div_etat);

        return glm::vec3 {
                (-view_direction / etai_div_etat) +
                (cos_theta_t / etai_div_etat - cos_theta_i) * normal
        };
    }
}