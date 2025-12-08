#include "renderer/path_tracing_renderer.h"

#include "util/frame.h"


glm::vec3 PathTracingRenderer::renderPixel(const glm::ivec2 &pixel_coord) {
    auto ray = camera.generateRay(pixel_coord, {rng.uniform(), rng.uniform()});
    glm::vec3 beta = {1, 1, 1};
    glm::vec3 L = {0, 0, 0};
    float q = 0.9;

    while (true) {
        auto hit_info = scene.intersect(ray);
        if (hit_info.has_value()) {
            if (rng.uniform() > q) {
                break;
            }
            L += beta * hit_info->material->emissive;

            Frame frame(hit_info->normal);
            glm::vec3 light_direction;

            if (hit_info->material->is_specular) {
                glm::vec3 view_direction = frame.localFromWorld(-ray.direction);
                light_direction = { -view_direction.x, view_direction.y, -view_direction.z };
                // brdf = hit_info->material->albedo / light_direction.y;
                // pdf = 1;
                // 局部坐标系下
                // glm::dot(light_direction, glm::vec3(0, 1, 0)) = light_direction.y
                // beta *= brdf * light_direction.y / pdf;
                beta *= hit_info->material->albedo;
            }else {
                do {
                    light_direction = { rng.uniform(), rng.uniform(), rng.uniform() };
                    // 0,1 -> -1,1
                    light_direction = light_direction * 2.f - 1.f;
                } while(glm::length(light_direction) > 1);
                if (light_direction.y < 0) {
                    light_direction.y = -light_direction.y;
                }
                // pdf = 1 / 2pi;
                // brdf = hit_info->material->albedo / pi;
                // 后面有brdf/pdf 所以pi会消掉
                // pdf = 0.5;
                // brdf = hit_info->material->albedo;
                // beta *= brdf * light_direction.y / pdf;
                light_direction = glm::normalize(light_direction);
                beta *= hit_info->material->albedo * light_direction.y * 2.f;
            }

            beta /= q;

            ray.origin = hit_info->hit_point;
            ray.direction = frame.worldFromLocal(light_direction);
        }else {
            break;
        }
    }

    return L;
}
