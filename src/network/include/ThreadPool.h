#pragma once
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

class ThreadPool{
    public:
     explicit ThreadPool(unsigned int size = std::thread::hardware_concurrency());
     ~ThreadPool();

     template <class F, class... Args>
     auto Add(F &&f, Args &&... args) -> std::future<typename std::result_of<F(Args...)>::type>;

    private:
     std::vector<std::thread> workers_;
     std::queue<std::function<void()>> tasks_;
     std::mutex queue_mutex_;
     std::condition_variable condition_variable_;
     bool stop_{false};
};

template <class F, class... Args> 
auto ThreadPool::Add(F &&f, Args &&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;
    auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        if(stop_){
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }
        workers_.emplace_back([task](){(*task)();});
    }
    condition_variable_.notify_one();
    return res;
}