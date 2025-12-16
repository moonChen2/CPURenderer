#pragma once

#include "camera/camera.h"
#include "shape/scene.h"

#define DEFINE_RENDERER(Name) \
    class Name##Renderer : public BaseRenderer { \
    public: \
        Name##Renderer(Camera &camera, const Scene &scene) : BaseRenderer(camera, scene) {} \
    private: \
        glm::vec3 renderPixel(const glm::ivec3 &pixel_coord) override; \
    }; \


class BaseRenderer {
public:
    BaseRenderer(Camera &camera, const Scene &scene) : camera(camera), scene(scene) {

    }

    void render(size_t spp, const std::filesystem::path &filename);
private:
    // ivec3 = x y ssp
    virtual glm::vec3 renderPixel(const glm::ivec3 &pixel_coord) = 0;
protected:
    Camera &camera;
    const Scene &scene;
};
