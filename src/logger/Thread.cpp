#include <semaphore.h> //信号量
#include "Thread.h"
#include "CurrentThread.h"

std::atomic_int32_t Thread::numCreated_(0);

Thread::Thread(ThreadFunc func, const std::string &name) :
    started_(false),  //未开始
    joined_(false),  //还未设置等待线程
    tid_(0),   //初始tid设置为0
    func_(std::move(func)),
    //名字是默认空字符串
    name_(name) {
        setDefaultName(); //设置线程索引编号和姓名
}

Thread::~Thread(){
    if(started_ && !joined_){  //线程启动并且未设置等待线程时才可以调用
        thread_->detach(); //设置线程分离（守护线程，不需要等待线程结束，不会产生孤儿线程）
    }
}

void Thread::start(){
    started_ = true;
    sem_t sem;
    sem_init(&sem, false, 0);
    //开启线程
    thread_ = std::shared_ptr<std::thread>(new std::thread([&](){
        tid_ = CurrentThread::tid(); //获取线程tid
        sem_post(&sem);   //v操作
        func_();   //开启一个新线程专门执行这个func_函数
    }));
    sem_wait(&sem); 
    /**
     * 这里必须等待获取上面新创建的线程的tid
     * 未获取到信息则不会执行sem_post，所以会被阻塞住
     * 如果不使用信号量操作，则别的线程访问tid时候，可能上面的线程还没有获取到tid
     */    
}

void Thread::join(){
    joined_ = true;
    thread_->join();//等待线程执行完毕
}

void Thread::setDefaultName(){
    int num = ++numCreated_;
    if (name_.empty()){
        char buf[32] = {0};
        snprintf(buf, sizeof(buf), "Thread%d", num);
        name_ = buf;
    }
}
