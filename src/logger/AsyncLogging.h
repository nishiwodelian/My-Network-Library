#pragma once
#include <functional>
#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>
#include "LogStream.h"
#include "Thread.h"
#include "noncopyable.h"

//异步日志
//AsyncLogging 提供后端线程，定时将 log 缓冲写到磁盘，维护缓冲及缓冲队列。
class AsyncLogging : noncopyable {
    public:
     AsyncLogging(const std::string& basename,
                    //off_t rollSize,
                    int flushInterval = 3);
     ~AsyncLogging(){
        if(running_){
            stop();
        }
     }

     void append(const char* logline, int len);  //前端用append写入日志

     void start(){
        running_ = true;
        thread_.start();
     }

     void stop(){
        running_ = false;
        cond_.notify_one();
        thread_.join();
     }



    private:
     void threadFunc();
     using Buffer = FixedBuffer<kLargeBuffer>;
     using BufferVector = std::vector<std::unique_ptr<Buffer>>;
     using BufferPtr = BufferVector::value_type;
     /*typedef FixedBuffer<kLargeBuffer> Buffer;
     typedef std::vector<std::shared_ptr<Buffer>> BufferVector;
     typedef std::shared_ptr<Buffer> BufferPtr;*/

     const int flushInterval_;
     std::atomic<bool> running_;  
     /*
     std::atomic对int, char, bool等数据结构进行原子性封装，在多线程环境中，
     对std::atomic对象的访问不会造成竞争-冒险。利用std::atomic可实现数据结构的无锁设计。
     */
    const std::string basename_;
    //const off_t roLLSize_;
    Thread thread_;
    std::mutex mutex_;
    std::condition_variable cond_;

    BufferPtr currentBuffer_;
    BufferPtr nextBuffer_;
    BufferVector buffers_;


};