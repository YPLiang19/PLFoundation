//
//  PLThread.hpp
//  PLFoundation
//
//  Created by yongpengliang on 2020/8/10.
//  Copyright © 2020 yongpengliang. All rights reserved.
//

#ifndef PLThread_hpp
#define PLThread_hpp

#include "PLFoundationCoreMacro.h"
#include "PLDate.h"
#include <functional>
#include <string>
#include <unordered_map>
#include <pthread.h>

PLFOUNDATON_NAMESPACE_BEGIN

class PLThread{
    
private:
    std::function<void(void)> _invoke;
    std::string _name;
    bool _cancelled = false;
    bool _active = false;
    bool _finished = false;
    pthread_t _pthreadID = NULL;
    std::unordered_map<void *, void *> *_thread_dictionary = nullptr;
    
    PLThread(){};
    static void initialize();
    void unregisterActiveThread();
    void registerActiveThread();
    
public:
    friend void * pl_threadLauncher(void *thread);
    
    static PLThread *currentThread();
    static void exit();
    static void sleepUntilDate(PLDate *date);
    static void sleepForTimeInterval(PLTimeInterval ti);
    static PLThread *mainThread();
    static void setThreadPriority(double priority);
    static double threadPriority();
    
    template<typename Object, typename Method>
    PLThread(Object *obj, Method method){
        _invoke = [obj, method](){
            (obj->*method)();
        };
    }
    
    template<typename Object, typename Method, typename Argument>
    PLThread(Object *obj, Method method, Argument arg){
        _invoke = [obj, method, arg](){
            (obj->*method)(arg);
        };
    }
    
    template<typename Object, typename Method, typename Argument1, typename Argument2>
    PLThread(Object *obj, Method method, Argument1 arg1, Argument2 arg2){
        _invoke = [obj, method, arg1, arg2](){
            (obj->*method)(arg1, arg2);
        };
    }
    
    virtual ~PLThread();
    
    virtual void setName(const char *name);
    
    virtual std::string name();
    
    virtual bool isCancelled();
    virtual void cancel();
    
    virtual bool isExecuting();
    
    virtual bool isFinished();
    
    virtual bool isMainThread();
    
    virtual void start();
    
    virtual void main();
    
    virtual std::unordered_map<void *, void *> *threadDictionary();
    
};

PLFOUNDATON_NAMESPACE_END

#endif /* PLThread_hpp */
