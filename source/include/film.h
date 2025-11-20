#pragma once

#include <vector>
#include <filesystem>
#include <thirdParty/glm/glm.hpp>

//相机胶片 存储每个pixel颜色
class Film {
public:
    size_t width, height;

    Film(size_t width, size_t height);

    void save(const std::filesystem::path &filename);

    glm::vec3 getPixel(size_t x, size_t y) {
        return pixels[y * width + x];
    };

    void setPixel(size_t x, size_t y, const glm::vec3 &color) {
        pixels[y * width + x] = color;
    }

private:
    std::vector<glm::vec3> pixels;
};


