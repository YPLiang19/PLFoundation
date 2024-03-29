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
#include "PLObject.h"
#include "PLDate.h"
#include <functional>
#include <string>
#include <unordered_map>
#include <pthread.h>
#include <memory>

PLFOUNDATON_NAMESPACE_BEGIN

class PLThread :  public PLObject, public std::enable_shared_from_this<PLThread>{
    
private:
    std::function<void(void)> _block;
    std::string _name;
    bool _cancelled = false;
    bool _active = false;
    bool _finished = false;
    pthread_t _pthreadID = NULL;
    std::unordered_map<void *, void *> *_thread_dictionary = nullptr;
    
    PLThread(){};
    
    PLThread(std::function<void(void)> block) : _block(block){}
    
    template<typename Object, typename Method>
    PLThread(std::shared_ptr<Object> objectPtr, Method method){
        _block = [objectPtr, method](){
            if (objectPtr) (objectPtr.get()->*method)();
        };
    }
       
    template<typename Object, typename Method, typename Argument>
    PLThread(std::shared_ptr<Object> objectPtr, Method method, Argument arg){
        _block = [objectPtr, method, arg](){
            if (objectPtr) (objectPtr.get()->*method)();
        };
    }
       
    template<typename Object, typename Method, typename Argument1, typename Argument2>
    PLThread(std::shared_ptr<Object> objectPtr, Method method, Argument1 arg1, Argument2 arg2){
        _block = [objectPtr, method, arg1, arg2](){
            if (objectPtr) (objectPtr.get()->*method)();
        };
    }
    
    template<typename Object, typename Method>
    PLThread(std::weak_ptr<Object> objectPtr, Method method){
        _block = [objectPtr, method](){
            if (!objectPtr.expired()) (objectPtr.lock().get()->*method)();
        };
    }
       
    template<typename Object, typename Method, typename Argument>
    PLThread(std::weak_ptr<Object> objectPtr, Method method, Argument arg){
        _block = [objectPtr, method, arg](){
            if (!objectPtr.expired()) (objectPtr.lock().get()->*method)();
        };
    }
       
    template<typename Object, typename Method, typename Argument1, typename Argument2>
    PLThread(std::weak_ptr<Object> objectPtr, Method method, Argument1 arg1, Argument2 arg2){
        _block = [objectPtr, method, arg1, arg2](){
            if (!objectPtr.expired()) (objectPtr.lock().get()->*method)();
        };
    }
    
    
    template<typename Object, typename Method>
    PLThread(Object *objectPtr, Method method){
        _block = [objectPtr, method](){
             if (objectPtr) (objectPtr->*method)();
        };
    }
       
    template<typename Object, typename Method, typename Argument>
    PLThread(Object *objectPtr, Method method, Argument arg){
        _block = [objectPtr, method, arg](){
             if (objectPtr) (objectPtr->*method)(arg);
        };
    }
       
    template<typename Object, typename Method, typename Argument1, typename Argument2>
    PLThread(Object *objectPtr, Method method, Argument1 arg1, Argument2 arg2){
        _block = [objectPtr, method, arg1, arg2](){
             if (objectPtr) (objectPtr->*method)(arg1, arg2);
        };
    }
       
    static void initialize();
    void unregisterActiveThread();
    void registerActiveThread();
    
public:
    friend void * pl_threadLauncher(void *thread);
    
    template<typename ObjectPtr, typename Method>
    static std::shared_ptr<PLThread> thread(ObjectPtr objectPtr, Method method){
        return std::shared_ptr<PLThread> (new PLThread(objectPtr, method));
    }
    
    template<typename ObjectPtr, typename Method, typename Argument>
    static std::shared_ptr<PLThread> thread(ObjectPtr objectPtr, Method method, Argument arg){
        return std::shared_ptr<PLThread> (new PLThread(objectPtr, method, arg));
    }
      
    template<typename ObjectPtr, typename Method, typename Argument1, typename Argument2>
    static std::shared_ptr<PLThread> thread(ObjectPtr objectPtr, Method method, Argument1 arg1, Argument2 arg2){
        return std::shared_ptr<PLThread> (new PLThread(objectPtr, method, arg1, arg2));
    }
    
    static std::shared_ptr<PLThread> thread(std::function<void(void)> block){
         return std::shared_ptr<PLThread> (new PLThread(block));
    }
    
    virtual ~PLThread();
    
    static std::shared_ptr<PLThread> currentThread();
    static void exit();
    static void sleepUntilDate(PLDate &date);
    static void sleepForTimeInterval(PLTimeInterval ti);
    static std::shared_ptr<PLThread> mainThread();
    static void setThreadPriority(double priority);
    static double threadPriority();
    
    
    
    virtual void setName(std::string name);
    
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
