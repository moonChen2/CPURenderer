#include "thread/thread_pool.h"

#include <cmath>

#include "util/profile.h"

ThreadPool thread_pool {};

void ThreadPool::WorkerThread(ThreadPool *master) {
    while(master->alive == 1){
        Task *task = master->getTask();
        if(task != nullptr){
            task->run();
            delete task;
            --master->pending_task_count;
        }else{
            std::this_thread::yield();
        }
    }
}

ThreadPool::ThreadPool(size_t thread_count) {
    alive = 1;
    pending_task_count = 0;
    if(thread_count == 0){
        thread_count = std::thread::hardware_concurrency();
    }

    for(size_t i = 0; i < thread_count; i++) {
        //this指针会用来初始化worker thread
        threads.push_back(std::thread(ThreadPool::WorkerThread, this));
    }
}


ThreadPool::~ThreadPool() {
    //等待所有任务完成
    wait();
    alive = 0;
    //等待所有线程结束
    //wait表示活干完 join保证人下班，都满足的时候公司才可以关门
    for(auto &thread : threads){
        thread.join();
    }
    threads.clear();
}

void ThreadPool::wait() const {
    while(pending_task_count > 0){
        //当前线程主动放弃CPU调度
        std::this_thread::yield();
    }
}


void ThreadPool::addTask(Task *task) {
    Guard guard(spin_lock);
    tasks.push(task);
}

Task *ThreadPool::getTask() {
    Guard guard(spin_lock);

    if(tasks.empty()){
        return nullptr;
    }

    Task *task = tasks.front();
    tasks.pop();
    return task;
}

class ParallelForTask : public Task {
public:
    ParallelForTask(size_t x, size_t y, size_t chunk_width, size_t chunk_height, const std::function<void(size_t, size_t)> &lambda)
        :x(x), y(y), chunk_width(chunk_width), chunk_height(chunk_height), lambda(lambda) {

    }
    void run() override {
        for (size_t idx_x = 0; idx_x < chunk_width; idx_x++) {
            for (size_t idx_y = 0; idx_y < chunk_height; idx_y++) {
                lambda(x + idx_x, y + idx_y);
            }
        }
    }
private:
    size_t x, y, chunk_width, chunk_height;
    std::function<void(size_t, size_t)> lambda;
};

//并行for循环
void ThreadPool::parallelFor(size_t width, size_t height, const std::function<void(size_t, size_t)> &lambda, bool complex) {
    PROFILE("parallelFor")
    Guard guard(spin_lock);
    //tile based 每个像素new 一个任务太多了
    //改为1个线程16个任务
    float chunk_width_float = static_cast<float>(width) / std::sqrt(threads.size());
    float chunk_height_float = static_cast<float>(height) / std::sqrt(threads.size());

    //如果是个比较复杂的任务，比如光追，就不要new出来那么多task
    //如果任务比较简单，比如film.save，只是存储rgb value，就一个像素一个任务即可
    if (complex) {
        chunk_width_float /= std::sqrt(16);
        chunk_height_float /= std::sqrt(16);
    }

    size_t chunk_width = std::ceil(chunk_width_float);
    size_t chunk_height = std::ceil(chunk_height_float);

    for (size_t x = 0; x < width; x += chunk_width) {
        for (size_t y = 0; y < height; y += chunk_height) {
            ++pending_task_count;
            //边界条件
            if ( x + chunk_width > width) {
                chunk_width = width - x;
            }
            if (y + chunk_height > height) {
                chunk_height = height - y;
            }
            tasks.push(new ParallelForTask(x, y, chunk_width, chunk_height, lambda));
        }
    }
}
