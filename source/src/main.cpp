#include <iostream>

#include "camera.h"
#include "thread_pool.h"
#include "film.h"
#include "sphere.h"
#include <iostream>

#include "model.h"

class SimpleTask : public Task{
    void run() override{
        std::cout<<" hello "<<std::endl;
    }
};

int main(){
    std::atomic<int> count = 0;
    ThreadPool threadPool {};

    Film film {1920,1080};
    Camera camera {film, {-1,0,0},{0,0,0},90};
    Sphere sphere{
        {0,0,0},
        0.5f
    };
    Model model {"../../models/simple_dragon.obj"};

    Shape &shape = model;

    glm::vec3 light_pos = {-1,2,1};
    threadPool.parallelFor(film.width, film.height, [&](size_t x, size_t y) {
        auto ray = camera.generateRay({x,y});
        auto hit_info = shape.intersect(ray);
        if (hit_info.has_value()) {
            auto L = glm::normalize(light_pos - hit_info->hit_point);
            float cosine = glm::dot(hit_info->normal, L);
            //这里传入的color是一个右值
            //set pixel中的color参数如果是左值引用就会报错
            //左值常引用可以接受右值
            film.setPixel(x,y,{cosine,cosine,cosine});
        }
        ++count;
        if (count % film.width == 0) {
            std::cout<<static_cast<float>(count) / (film.width * film.height)<<std::endl;
            //std::cout<<count<<std::endl;
        }
    });

    threadPool.wait();

    film.save("../../test.ppm");
    //threadPool.addTask(new SimpleTask());
}