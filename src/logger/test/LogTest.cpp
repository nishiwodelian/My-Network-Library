#include "AsyncLogging.h"
#include "Logging.h"
#include <time.h>
#include <sys/time.h>

#include <stdio.h>
#include <unistd.h>

AsyncLogging* g_asynclog = NULL;

inline AsyncLogging* getAsynclog(){
    return g_asynclog;
}

void test_Logging(){
    LOG_INFO << "info";
    LOG_WARN << "warn";
    LOG_ERROR << "error";

    const int n=10;
    for(int i=0; i< n; ++i){
        LOG_INFO << "Hello, " << i << " abc...xyz";
    }
 }

 void test_AsyncLogging(){
    const int n= 1024;
    for(int i=0;i<n;++i){
        LOG_INFO << "Hello, " << i << " abc...xyz";
    }
 }

 void asyncLog(const char* msg, int len){
    AsyncLogging* logging = getAsynclog();
    if(logging){
        logging->append(msg, len);
    }
 }

 int main(int argc, char* argv[]){
    printf("pid = %d\n", getpid());
    AsyncLogging log(::basename(argv[0]));
    LOG_INFO << argv[0];
    test_Logging();
    
    sleep(1);
    //printf("2");
    g_asynclog = &log;
    Logger::setOutput(asyncLog);   //为Logger设置输出的回调，重新设置输出位置
    //printf("3");
    log.start();  //开启日志的后端线程
    test_Logging();
    test_AsyncLogging();

    sleep(1);
    log.stop();
    return 0;
 }
