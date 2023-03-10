#include "AsyncLogging.h"
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <functional>
#include "LogFile.h"

AsyncLogging::AsyncLogging(const std::string& logFileName,
                           //off_t rollSize,
                           int flushInterval )
    : flushInterval_(flushInterval), 
    running_(false),
    basename_(logFileName),
    //roLLSize_(rollSize),
    thread_(std::bind(&AsyncLogging::threadFunc, this), "Logging"),
    mutex_(),
    cond_(),
    currentBuffer_(new Buffer),
    nextBuffer_(new Buffer),
    buffers_(){
        currentBuffer_->bzero();
        nextBuffer_->bzero();
        buffers_.reserve(16);
    }

    void AsyncLogging::append(const char* logline, int len){
        //lock在构造函数中自动绑定它的互斥并加锁，在析构函数中解锁，大大减少了死锁的风险
        std::lock_guard<std::mutex> lock(mutex_);
        //缓冲区剩余空间足够则直接写入
        if(currentBuffer_->avail() > len){
            currentBuffer_->append(logline, len);
        }else{
            //当前缓冲区空间不够，将新信息写入备用缓冲区
            buffers_.push_back(std::move(currentBuffer_));
            //currentBuffer_.reset();
            if(nextBuffer_){
                currentBuffer_ = std::move(nextBuffer_);
            }else{//备用缓冲区也不够时，重新分配缓冲区，这种情况挺少见的
                currentBuffer_.reset(new Buffer);
            }
            currentBuffer_->append(logline, len);
            //唤醒写入磁盘的后端线程
            cond_.notify_one();
        }
    }

    void AsyncLogging::threadFunc(){
        //output有写入磁盘的接口
        LogFile output(basename_, false);
        //后端缓冲区，用于归还前端的缓冲区，currentBuffer  nextBuffer
        BufferPtr newBuffer1(new Buffer);
        BufferPtr newBuffer2(new Buffer);
        newBuffer1->bzero();
        newBuffer2->bzero();
        //缓冲区数组置为16个，用于和前端缓冲区数据进行交换
        BufferVector buffersToWrite;
        buffersToWrite.reserve(16);
        while(running_){
            assert(newBuffer1 && newBuffer1->length() == 0);
            assert(newBuffer2 && newBuffer2->length() == 0);
            assert(buffersToWrite.empty());

            {
                std::unique_lock<std::mutex> lock(mutex_);//互斥锁保护，这样别的线程在这段时间无法向前端Buffer数组写入数据
                if(buffers_.empty()){
                    cond_.wait_for(lock, std::chrono::seconds(3));//等待三秒也会接触阻塞
                }
                //此时正使用的buffer也放入buffer数组中，没写完也放进去，避免等待太久才刷新一次
                buffers_.push_back(std::move(currentBuffer_));
                currentBuffer_ = std::move(newBuffer1);
                buffersToWrite.swap(buffers_);
                if(!nextBuffer_){
                    nextBuffer_ = std::move(newBuffer2);
                }
            }

            //遍历所有buffer，将其写入文件
            for(const auto& buffer : buffersToWrite){
                output.append(buffer->data(), buffer->length());
            }
            //只保留两个缓冲区
            if(buffersToWrite.size() > 2){
                buffersToWrite.resize(2);
            }
            //归还newBuffer1缓冲区
            if(!newBuffer1){
                newBuffer1 = std::move(buffersToWrite.back());
                buffersToWrite.pop_back();
                newBuffer1->reset();
            }
            //归还newBuffer2缓冲区
            if(!newBuffer2){
                newBuffer2 = std::move(buffersToWrite.back());
                buffersToWrite.pop_back();
                newBuffer2->reset();
            }
            buffersToWrite.clear(); //清空后端缓冲区队列
            output.flush();  //清空文件缓冲区
        }
        output.flush();
    }
