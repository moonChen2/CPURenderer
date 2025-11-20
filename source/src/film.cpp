#include <fstream>
#include "film.h"


Film::Film(size_t width, size_t height) : width(width), height(height) {
    pixels.resize(width *height);
}


void Film::save(const std::filesystem::path &filename) {
    //PPM
    //P3
    //1920 1080
    //255
    //...rgb
    std::ofstream file(filename);

    file<<"P3\n"<<width<<' '<<height<<"\n255\n";

    for(size_t y = 0; y < height; y++){
        for(size_t x = 0; x < width; x++){
            const glm::vec3 &color = getPixel(x,y);
            glm::ivec3 color_i = glm::clamp(color * 255.f, 0.f, 255.f);
            file<<color_i.x<<' '<<color_i.y<<' '<<color_i.z<<' '<< '\n';
        }
    }

}