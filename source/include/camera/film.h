#pragma once

#include <vector>
#include <filesystem>
#include <thirdParty/glm/glm.hpp>
#include "util/rgb.h"

struct Pixel{
    glm::vec3 color {0,0,0};
    int sample_count {0};
};

//相机胶片 存储每个pixel颜色
class Film {
public:
    size_t width, height;

    Film(size_t width, size_t height);

    void save(const std::filesystem::path &filename);

    Pixel getPixel(size_t x, size_t y) {
        return pixels[y * width + x];
    };

    void addSample(size_t x, size_t y, const glm::vec3 &color){
        pixels[y * width + x].color += color;
        pixels[y * width + x].sample_count ++;
    }

    void clear(){ pixels.clear(); pixels.resize(width * height); }
private:
    // 物理意义上的光照强度，线性空间，0-1
    std::vector<Pixel> pixels;
};


