#include <iostream>
#include <chrono>
#include <cstdlib>
#include <pthread.h>
#include <time.h>
#include "../SkipList.h"

#define NUM_THREADS 1
#define TEST_COUNT 100000
SkipList<int, std::string> skipList(18);

void *insertElement(void* threadid) {
    long tid;
    tid = (long)threadid;
    std::cout << tid << std::endl;
    int tmp = TEST_COUNT / NUM_THREADS ;
    for(int i=tid*tmp, count = 0; count < tmp ; i++) {
        count++;
        skipList.insertElement(rand() % TEST_COUNT, "a");
    }
    pthread_exit(NULL);
}

void *getElement(void* threadid) {
    long tid;
    std::string ret;
    tid = (long) threadid;
    std::cout << tid << std::endl;
    int tmp = TEST_COUNT/NUM_THREADS;
    for(int i=tid*tmp, count=0; count < tmp ; i++) {
        count++;
        skipList.searchElement(rand() % TEST_COUNT, ret);
    }
    pthread_exit(NULL);
}

int main() {
    srand (time(NULL));
    {
        pthread_t threads[NUM_THREADS];
        int rc;
        int i;
       /* for(i = 0; i< NUM_THREADS; i++){
            std::cout << "main(): creating thread, " << i << std::endl;
            rc = pthread_create(&threads[i], NULL, insertElement, (void *)i);
            if(rc) {
                std::cout << "Error:unable to create thread," << rc << std::endl;
                exit(-1);
            }
        }
        void *ret;
        for( i = 0 ; i<NUM_THREADS; i++){
            if(pthread_join(threads[i], &ret) != 0) {
                perror("pthread_create() error");
                exit(3);
            }
        }*/
        auto start = std::chrono::high_resolution_clock::now();
        for(i = 0; i< NUM_THREADS; i++){
            std::cout << "main(): creating thread, " << i << std::endl;
            rc = pthread_create(&threads[i], NULL, getElement, (void *)i);
            if(rc) {
                std::cout << "Error:unable to create thread," << rc << std::endl;
                exit(-1);
            }
        }
        void *ret2;
        for( i = 0 ; i<NUM_THREADS; i++){
            if(pthread_join(threads[i], &ret2) != 0) {
                perror("pthread_create() error");
                exit(3);
            }
        }
        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = finish - start;
        std::cout << "insert elapsed:" << elapsed.count() << std::endl;
        pthread_exit(NULL);
        return 0;
    }
}