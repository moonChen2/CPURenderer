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

    Film film {1920,1080};

    for(int i = 0;i<100;i++){
        for(int j = 0;j<200;j++){
            film.setPixel(j,i,{0.5,0.7,0.2});
        }
    }

    film.save("../../test.ppm");

    ThreadPool threadPool {};
    threadPool.addTask(new SimpleTask());
}