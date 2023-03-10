#include "ThreadPool.h"

ThreadPool::ThreadPool(unsigned int size){
    for(unsigned int i=0;i<size;++i){ //  启动size个线程
        workers_.emplace_back(std::thread([this](){ //定义每个线程的工作函数
            while(true){
                std::function<void()> task;  //这里锁住的共享资源是tasks
                {//在这个{}作用域内对std::mutex加锁，出了作用域会自动解锁，不需要调用unlock()
                    std::unique_lock<std::mutex> lock(queue_mutex_);
                    condition_variable_.wait(lock, [this](){//等待条件变量，条件为任务队列不为空或线程池停止
                        return stop_ || !tasks_.empty();//相当于flag验证，为了防止假醒情况,而且当任务队列为空时CPU不应该不断轮询耗费CPU资源
                    });
                    if(stop_&& tasks_.empty()) return;//任务队列为空并且线程池停止，退出线程
                    task = tasks_.front();//从任务队列头取出一个任务
                    tasks_.pop();
                }
                task();//执行任务
            }
        }));
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        stop_ = true;
    }
    condition_variable_.notify_all();
    for(std::thread &th : workers_){
        if(th.joinable()){ //当线程对象为可加入的。该线程是可执行线程
            th.join();//会主动地等待线程的终止。当新的线程终止时，join()会清理相关的资源，然后返回，调用线程再继续向下执行。
        }
    }
}