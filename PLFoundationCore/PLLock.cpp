//
//  PLLock.cpp
//  PLFoundation
//
//  Created by yongpengliang on 2020/8/7.
//  Copyright © 2020 yongpengliang. All rights reserved.
//

#include "PLLock.h"

static pthread_mutexattr_t attr_normal;
static pthread_mutexattr_t attr_reporting;
static pthread_mutexattr_t attr_recursive;

PLFOUNDATON_NAMESPACE_BEGIN

#pragma mark - PLLock

PLLock::PLLock(){
    static pl_dispatch_once_t onceToken;
    pl_dispatch_once(&onceToken, [](){
        pthread_mutexattr_init(&attr_normal);
        pthread_mutexattr_settype(&attr_normal, PTHREAD_MUTEX_NORMAL);
    });
    pthread_mutex_init(&_mutex, &attr_reporting);
}

void PLLock::setName(std::string &name){
    _name = name;
}

std::string& PLLock::name(){
    return _name;
}

void PLLock::lock(){
    pthread_mutex_lock(&_mutex);
}

bool PLLock::tryLock(){
    int err = pthread_mutex_trylock(&_mutex);
    return err == 0;
}

bool PLLock::lockBeforeDate(PLDate *limit){
    do {
        int err = pthread_mutex_trylock(&_mutex);
        if (0 == err) {
            return true;
        }
        sched_yield();
    } while (limit->timeIntervalSinceNow() >0);
    return 0;
}

void PLLock::unLock(){
    pthread_mutex_unlock(&_mutex);
}

PLLock::~PLLock(){
    pthread_mutex_destroy(&_mutex);
}

#pragma mark - PLRecursiveLock


PLRecursiveLock::PLRecursiveLock(){
    static pl_dispatch_once_t onceToken;
    pl_dispatch_once(&onceToken, [](){
        pthread_mutexattr_init(&attr_recursive);
        pthread_mutexattr_settype(&attr_recursive, PTHREAD_MUTEX_RECURSIVE);
    });
    pthread_mutex_init(&_mutex, &attr_reporting);
}

void PLRecursiveLock::setName(std::string &name){
    _name = name;
}

std::string& PLRecursiveLock::name(){
    return _name;
}

void PLRecursiveLock::lock(){
    pthread_mutex_lock(&_mutex);
}

bool PLRecursiveLock::tryLock(){
    int err = pthread_mutex_trylock(&_mutex);
    return err == 0;
}

bool PLRecursiveLock::lockBeforeDate(PLDate *limit){
    do {
        int err = pthread_mutex_trylock(&_mutex);
        if (0 == err) {
            return true;
        }
        sched_yield();
    } while (limit->timeIntervalSinceNow() >0);
    return 0;
}

void PLRecursiveLock::unLock(){
    pthread_mutex_unlock(&_mutex);
}

PLRecursiveLock::~PLRecursiveLock(){
    pthread_mutex_destroy(&_mutex);
}

#pragma mark - PLCondition

PLFOUNDATON_NAMESPACE_END





