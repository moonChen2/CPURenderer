#include <iostream>
#include "thirdParty/glm/glm.hpp"
#include "thread_pool.h"
#include "film.h"

class SimpleTask : public Task{
    void run() override{
        std::cout<<" hello "<<std::endl;
    }
};

int main(){
    ThreadPool threadPool {};
    Film film {1920,1080};

    threadPool.parallelFor(200, 100, [&](size_t x, size_t y) {
        //这里传入的color是一个右值
        //set pixel中的color参数如果是左值引用就会报错
        //左值常引用可以接受右值
        film.setPixel(x,y,{0.5,0.7,0.2});
    });

    threadPool.wait();

    film.save("../../test.ppm");
    threadPool.addTask(new SimpleTask());
}