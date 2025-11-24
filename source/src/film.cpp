#include <fstream>
#include "film.h"
#include <iostream>

Film::Film(size_t width, size_t height) : width(width), height(height) {
    pixels.resize(width * height);
}


void Film::save(const std::filesystem::path &filename) {
    //PPM
    //P6
    //1920 1080
    //255
    //...rgb
    std::ofstream file(filename, std::ios::binary); //187ms -> 120ms

    file<<"P6\n"<<width<<' '<<height<<"\n255\n";

    for(size_t y = 0; y < height; y++){
        for(size_t x = 0; x < width; x++){
            //gama correction
            auto pixel = getPixel(x,y);
            std::cout<<pixel.color.r<<" "<<pixel.color.g<<" "<<pixel.color.b<<" "<<std::endl;

            RGB rgb{pixel.color / static_cast<float>(pixel.sample_count)};
            //r8g8b8
            file<<static_cast<uint8_t>(rgb.r)<<static_cast<uint8_t>(rgb.g)<<static_cast<uint8_t>(rgb.b);
        }
    }

}