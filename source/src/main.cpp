#include <iostream>

#include "camera.h"
#include "thread_pool.h"
#include "film.h"
#include "sphere.h"

class SimpleTask : public Task{
    void run() override{
        std::cout<<" hello "<<std::endl;
    }
};

int main(){
    ThreadPool threadPool {};

    Film film {1920,1080};
    Camera camera {film, {0,0,1},{0,0,0},90};
    Sphere sphere{
        {0,0,0},
        0.5f
    };
    glm::vec3 light_pos = {1,1,1};
    threadPool.parallelFor(film.width, film.height, [&](size_t x, size_t y) {
        auto ray = camera.generateRay({x,y});
        auto result = sphere.intersect(ray);
        if (result.has_value()) {
            auto hit_point = ray.hit(result.value());
            auto normal = glm::normalize(hit_point - sphere.center);

            auto L = glm::normalize(light_pos - hit_point);
            float cosine = glm::dot(normal, L);
            //这里传入的color是一个右值
            //set pixel中的color参数如果是左值引用就会报错
            //左值常引用可以接受右值
            film.setPixel(x,y,{cosine,cosine,cosine});
        }
    });

    threadPool.wait();

    film.save("../../test.ppm");
    threadPool.addTask(new SimpleTask());
}