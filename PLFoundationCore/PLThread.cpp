//
//  PLThread.cpp
//  PLFoundation
//
//  Created by yongpengliang on 2020/8/10.
//  Copyright © 2020 yongpengliang. All rights reserved.
//

#include "PLThread.h"
#include <pthread.h>

PLFOUNDATON_NAMESPACE_BEGIN

static PLThread *defaultThread = nullptr;
static bool keyInitialized = false;
static pthread_key_t thread_object_key;
static std::unordered_map<void *, void *> *activeThreads = nullptr;

static void exitedThread(void *thread){
    
}


void PLThread::initialize(){
    static pl_dispatch_once_t onceToken;
    pl_dispatch_once(&onceToken, [](){
        if (keyInitialized == false) {
            pthread_key_create(&thread_object_key, exitedThread);
            keyInitialized = true;
        }
        PLThread::currentThread();
    });
    
}

PLThread *PLThread::currentThread(){
    return nullptr;
}



void PLThread::setName(const char *name){
    _name = name;
}

std::string PLThread::name(){
    return _name;
}

bool PLThread::isMainThread(){
    return true;
}

bool PLThread::isCancelled(){
    return _cancelled;
}



PLFOUNDATON_NAMESPACE_END
