#include "thread_pool.h"

void ThreadPool::WorkerThread(ThreadPool *master) {
    while(master->alive == 1){
        Task *task = master->getTask();
        if(task != nullptr){
            task->run();
        }else{
            std::this_thread::yield();
        }
    }
}

ThreadPool::ThreadPool(size_t thread_count) {
    alive = 1;
    if(thread_count == 0){
        thread_count = std::thread::hardware_concurrency();
    }

    for(size_t i = 0; i < thread_count; i++) {
        //this指针会用来初始化worker thread
        threads.push_back(std::thread(ThreadPool::WorkerThread, this));
    }
}


ThreadPool::~ThreadPool() {
    wait();
    alive = 0;
    for(auto &thread : threads){
        thread.join();
    }
    threads.clear();
}

void ThreadPool::wait() const {
    //等待所有任务完成
    while(!tasks.empty()){
        //当前线程主动放弃CPU调度
        std::this_thread::yield();
    }
}


void ThreadPool::addTask(Task *task) {
    Guard guard(spin_lock);

    tasks.push_back(task);
}

Task *ThreadPool::getTask() {
    Guard guard(spin_lock);

    if(tasks.empty()){
        return nullptr;
    }

    Task *task = tasks.front();
    tasks.pop_front();
    return task;
}

class ParallelForTask : public Task {
public:
    ParallelForTask(size_t x, size_t y, const std::function<void(size_t, size_t)> &lambda)
        :x(x),y(y),lambda(lambda) {

    }
    void run() override {
        lambda(x, y);
    }
private:
    size_t x, y;
    std::function<void(size_t, size_t)> lambda;
};

//并行for循环
void ThreadPool::parallelFor(size_t width, size_t height, const std::function<void(size_t, size_t)> &lambda) {
    Guard guard(spin_lock);

    for (size_t x = 0; x < width; x++) {
        for (size_t y = 0; y < height; y++) {
            tasks.push_back(new ParallelForTask(x, y, lambda));
        }
    }
}
