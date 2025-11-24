#pragma once

#include "spin_lock.h"
#include <vector>
#include <thread>
#include <queue>
#include <functional>

class Task{
public:
    virtual void run() = 0;
};

//仅支持主线程添加任务
class ThreadPool{
public:
    static void WorkerThread(ThreadPool *master);

    ThreadPool(size_t thread_count = 0);
    ~ThreadPool();

    void wait() const;
    void addTask(Task *task);
    Task *getTask();

    void parallelFor(size_t width, size_t height, const std::function<void(size_t, size_t)> &lambda);

private:
    std::atomic<int> alive;
    std::atomic<int> pending_task_count;
    std::vector<std::thread> threads;

    std::queue<Task *> tasks;
    SpinLock spin_lock{};
};


extern ThreadPool thread_pool;
