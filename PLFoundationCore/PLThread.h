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
#include <functional>
#include <string>
#include <unordered_map>

PLFOUNDATON_NAMESPACE_BEGIN

class PLThread{
private:
    std::function<void(void)> _invoke;
    std::string _name;
    bool _cancelled;
    bool _active;
    bool _finished;
    std::unordered_map<void *, void *> *_thread_dictionary;
    
    static void initialize();
    
    
public:
    
    static PLThread *currentThread();
    
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
    
    void setName(const char *name);
    
    std::string name();
    
    bool isCancelled();
    
    bool isExecuting();
    
    bool isFinished();
    
    bool isMainThread();
    
    void start();
    
    void main();
    
    std::unordered_map<void *, void *> *threadDictionary();
    
    
    
    
    
};

PLFOUNDATON_NAMESPACE_END

#endif /* PLThread_hpp */
