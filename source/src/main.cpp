#include <iostream>

#include "camera.h"
#include "thread_pool.h"
#include "film.h"
#include "sphere.h"
#include <iostream>
#include "plane.h"
#include "scene.h"
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
    Camera camera {film, {-1.6,0,0},{0,0,0},90};
    Sphere sphere{
        {0,0,0},
        0.5f
    };
    Plane plane {
        {0,0,0},
        {0,1,0}
    };
    Model model {"../../models/simple_dragon.obj"};

    Scene scene {};
    scene.addShape(&model, {0,0,0}, {0,0,0}, {1,3,2});
    scene.addShape(&sphere, {0,0,1.5}, {0,0,0}, {0.3, 0.3,0.3});
    scene.addShape(&plane, {0,-0.5,0});

    glm::vec3 light_pos = {-1,2,1};
    threadPool.parallelFor(film.width, film.height, [&](size_t x, size_t y) {
        auto ray = camera.generateRay({x,y});
        auto hit_info = scene.intersect(ray);
        if (hit_info.has_value()) {
            auto L = glm::normalize(light_pos - hit_info->hit_point);
            float cosine = glm::dot(hit_info->normal, L);
            //这里传入的color是一个右值
            //set pixel中的color参数如果是左值引用就会报错
            //左值常引用可以接受右值
            film.setPixel(x,y,{cosine,cosine,cosine});
        }
        int n = ++count;
        if (n % film.width == 0) {
            std::cout<<static_cast<float>(n) / (film.width * film.height)<<std::endl;
            //std::cout<<count<<std::endl;
        }
    });

    threadPool.wait();

    film.save("../../test.ppm");
    //threadPool.addTask(new SimpleTask());
}