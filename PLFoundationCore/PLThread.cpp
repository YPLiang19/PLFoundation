//
//  PLThread.cpp
//  PLFoundation
//
//  Created by yongpengliang on 2020/8/10.
//  Copyright © 2020 yongpengliang. All rights reserved.
//

#include "PLThread.h"
#include "PLFoundation_private.h"
#include <iostream>
#include <unistd.h>

PLFOUNDATON_NAMESPACE_BEGIN


#define PTHREAD_MAX_PRIORITY 31
#define PTHREAD_MIN_PRIORITY 0

#pragma mark - static variables

static PLThread *defaultThread = nullptr;
static bool keyInitialized = false;
static pthread_key_t thread_object_key;

#pragma mark - C function



static void exitedThread(void *thread){
    if (thread != defaultThread) {
        if (thread == NULL) {
            thread = pthread_getspecific(thread_object_key);
            if (thread == NULL) {
                return;
            }
            fprintf(stderr, "abnormally destroyed thread %p", thread);
        }
    }
}




void * pl_threadLauncher(void *thread){
    PLThread *t = (PLThread *)thread;
    t->registerActiveThread();
    //todo 发送线程开始通知
    t->main();
    PLThread::exit();
    return nullptr;
}

void sleepUntilIntervalSinceReferenceDate(PLTimeInterval when) {
    PLTimeInterval delay;
    delay = when - PLPrivateTimeNow();
    if (delay <= 0.0) {
       sched_yield();
       return;
    }
    
    while (delay > 30.0*60.0) {
       // sleep 30 minutes
      sleep(30*60);
      delay = when -  PLPrivateTimeNow();
    }

    if (delay > 0) {
       struct timespec request;
       struct timespec remainder;

       request.tv_sec = (time_t)delay;
       request.tv_nsec = (long)((delay - request.tv_sec) * 1000000000);
       remainder.tv_sec = 0;
       remainder.tv_nsec = 0;

      /*
       * With nanosleep, we can restart the sleep after a signal by using
       * the remainder information ... so we can be sure to sleep to the
       * desired limit without having to re-generate the delay needed.
       */
       while (nanosleep(&request, &remainder) < 0 && (remainder.tv_sec > 0 || remainder.tv_nsec > 0)) {
          request.tv_sec = remainder.tv_sec;
          request.tv_nsec = remainder.tv_nsec;
          remainder.tv_sec = 0;
          remainder.tv_nsec = 0;
       }
    }
}

#pragma mark - static member function

void PLThread::initialize(){
    static pl_dispatch_once_t onceToken;
    pl_dispatch_once(&onceToken, [](){
        if (keyInitialized == false) {
            pthread_key_create(&thread_object_key, exitedThread);
            keyInitialized = true;
        }
    });
    
}

PLThread *PLThread::currentThread(){
    PLThread::initialize();
    PLThread *thread = (PLThread *)pthread_getspecific(thread_object_key);
    if (thread == nullptr) {
        thread = new PLThread();
        thread->_active = true;
        thread->registerActiveThread();
        if (defaultThread == nullptr || pthread_main_np() == 1) {
            //todo 主线程不能被释放问题处理
            defaultThread = thread;
        }
    }
    return thread;
}

void PLThread::exit(){
    PLThread *t = PLThread::currentThread();
    if (t->_active) {
        t->unregisterActiveThread();
        if (t == defaultThread) {
            ::exit(0);
        } else {
            pthread_exit(NULL);
        }
    }
}

void PLThread::sleepUntilDate(PLDate *date){
    sleepUntilIntervalSinceReferenceDate(date->timeIntervalSinceReferenceDate());
}

void PLThread::sleepForTimeInterval(PLTimeInterval ti){
    sleepUntilIntervalSinceReferenceDate(PLPrivateTimeNow() + ti);
}

PLThread *PLThread::mainThread(){
    return defaultThread;
}


void PLThread::setThreadPriority(double pri){
    int    policy;
    struct sched_param param;

    // Clamp pri into the required range.
    if (pri > 1) { pri = 1; }
    if (pri < 0) { pri = 0; }

    // Scale pri based on the range of the host system.
    pri *= (PTHREAD_MAX_PRIORITY - PTHREAD_MIN_PRIORITY);
    pri += PTHREAD_MIN_PRIORITY;

    pthread_getschedparam(pthread_self(), &policy, &param);
    param.sched_priority = pri;
    pthread_setschedparam(pthread_self(), policy, &param);
}

double PLThread::threadPriority(){
    double pri = 0;
    int policy;
    struct sched_param param;

    pthread_getschedparam(pthread_self(), &policy, &param);
    pri = param.sched_priority;
    // Scale pri based on the range of the host system.
    pri -= PTHREAD_MIN_PRIORITY;
    pri /= (PTHREAD_MAX_PRIORITY - PTHREAD_MIN_PRIORITY);
    return pri;
}

#pragma mark - constructor & destructor

PLThread::~PLThread(){
    if (_thread_dictionary) {
        delete _thread_dictionary;
    }
}

#pragma mark - normal member function
void PLThread::setName(const char *name){
    _name = name;
}

std::string PLThread::name(){
    return _name;
}

bool PLThread::isCancelled(){
    return _cancelled;
}

void PLThread::cancel(){
    _cancelled = true;
}

bool PLThread::isExecuting(){
    return _active;
}

bool PLThread::isFinished(){
    return _finished;
}

bool PLThread::isMainThread(){
    return this == defaultThread;
}

void PLThread::start(){
    if (_active) {
        fprintf(stderr, "%p::start called on active thread", this);
        return;
    }
    
    if (_cancelled) {
        fprintf(stderr, "%p::start called on cancelled thread", this);
        return;
    }
    
    if (_finished) {
        fprintf(stderr, "%p::start called on finished thread", this);
        return;
    }
    
    //todo  retain this
    
    _active = true;
    errno = 0;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (pthread_create(&_pthreadID, &attr, pl_threadLauncher, (void *)this)) {
        //todo 替换成PLError
        fprintf(stderr, "Unable to detach thread (last error %d)", errno);
    }
}

void PLThread::main(){
    if (_active == false) {
        fprintf(stderr, "%p::main should call on active thread", this);
        return;
    }
    _invoke();
}

std::unordered_map<void *, void *> * PLThread::threadDictionary(){
    if (_thread_dictionary == nullptr) {
        _thread_dictionary = new std::unordered_map<void *, void*>();
    }
    return _thread_dictionary;
}


 void PLThread::unregisterActiveThread(){
    if (_active) {
        _active = false;
        _finished = true;
        //todo 发送线程退出通知和runloop处理
        pthread_setspecific(thread_object_key, NULL);
    }
}

 void PLThread::registerActiveThread(){
    pthread_setspecific(thread_object_key, (void *)this);
}




PLFOUNDATON_NAMESPACE_END
