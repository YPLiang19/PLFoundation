//
//  PLLock.cpp
//  PLFoundation
//
//  Created by yongpengliang on 2020/8/7.
//  Copyright © 2020 yongpengliang. All rights reserved.
//

#include "PLLock.h"
#include <math.h>
#include <errno.h>
#include <iostream>

static pthread_mutexattr_t attr_normal;
static pthread_mutexattr_t attr_reporting;
static pthread_mutexattr_t attr_recursive;

PLFOUNDATON_NAMESPACE_BEGIN

#pragma mark - PLLock

void PLLock::initialize(){
    static pl_dispatch_once_t onceToken;
    pl_dispatch_once(&onceToken, [](){
        pthread_mutexattr_init(&attr_normal);
        pthread_mutexattr_settype(&attr_normal, PTHREAD_MUTEX_NORMAL);
    });
}

PLLock::PLLock(){
    PLLock::initialize();
    pthread_mutex_init(&_mutex, &attr_reporting);
}

PLLock::~PLLock(){
    pthread_mutex_destroy(&_mutex);
}

void PLLock::setName(const char *name){
    _name = name;
}

std::string PLLock::name(){
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

void PLLock::unlock(){
    pthread_mutex_unlock(&_mutex);
}

#pragma mark - PLRecursiveLock

void PLRecursiveLock::initialize(){
    static pl_dispatch_once_t onceToken;
    pl_dispatch_once(&onceToken, [](){
        pthread_mutexattr_init(&attr_recursive);
        pthread_mutexattr_settype(&attr_recursive, PTHREAD_MUTEX_RECURSIVE);
    });
}

PLRecursiveLock::PLRecursiveLock(){
    PLRecursiveLock::initialize();
    pthread_mutex_init(&_mutex, &attr_reporting);
}

PLRecursiveLock::~PLRecursiveLock(){
    pthread_mutex_destroy(&_mutex);
}

void PLRecursiveLock::setName(const char *name){
    _name = name;
}

std::string PLRecursiveLock::name(){
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

void PLRecursiveLock::unlock(){
    pthread_mutex_unlock(&_mutex);
}

#pragma mark - PLCondition

void PLCondition::initialize(){
    static pl_dispatch_once_t onceToken;
    pl_dispatch_once(&onceToken, [](){
        pthread_mutexattr_init(&attr_reporting);
        pthread_mutexattr_settype(&attr_reporting, PTHREAD_MUTEX_ERRORCHECK);
    });
}

PLCondition::PLCondition(){
    initialize();
    pthread_cond_init(&_condition, NULL);
    pthread_mutex_init(&_mutex, &attr_reporting);
}

PLCondition::~PLCondition(){
    pthread_cond_destroy(&_condition);
    pthread_mutex_destroy(&_mutex);
}

void PLCondition::setName(const char *name){
    _name = name;
}

std::string& PLCondition::name(){
    return _name;
}

void PLCondition::wait(){
    pthread_cond_wait(&_condition, &_mutex);
}

void PLCondition::lock(){
    pthread_mutex_lock(&_mutex);
}

void PLCondition::unlock(){
    pthread_mutex_unlock(&_mutex);
}

bool PLCondition::waitUntilDate(PLDate *limit){
    PLTimeInterval ti = limit->timeIntervalSince1970();
    double secs, subsecs;
    struct timespec timeout;
    int retVal = 0;
    
    subsecs = modf(ti, &secs);
    timeout.tv_sec = secs;
    timeout.tv_nsec = subsecs * 1e9;
    
    retVal = pthread_cond_timedwait(&_condition, &_mutex, &timeout);
    if (retVal == 0) {
        return true;
    }
    
    if (retVal == ETIMEDOUT) {
        return false;
    }
    
    if (retVal == EINVAL) {
        std::clog << "Invalid arguments to pthread_cond_timedwait" << std::endl;
    }
    
    return false;
}

void PLCondition::signal(){
    pthread_cond_signal(&_condition);
}

void PLCondition::broadcast(){
    pthread_cond_broadcast(&_condition);
}

bool PLCondition::tryLock(){
    int err = pthread_mutex_trylock(&_mutex);
    return err == 0;
}

bool PLCondition::lockBeforeDate(PLDate *limit){
    do {
        int err = pthread_mutex_trylock(&_mutex);
        if (0 == err) {
            return true;
        }
        sched_yield();
    } while (limit->timeIntervalSinceNow() >0);
    return false;
}


#pragma mark - PLConditionLock

void PLConditionLock::initialize(){
    static pl_dispatch_once_t onceToken;
    pl_dispatch_once(&onceToken, [](){
        
    });
}

PLConditionLock::PLConditionLock(int value): _condition_value(value){
    _condition = new PLCondition();
    char buf[64];
    sprintf(buf, "condition-for-lock-%p", this);
    _condition->setName(buf);
}

PLConditionLock::~PLConditionLock(){
    if (_condition) {
        delete _condition;
    }
}

void PLConditionLock::setName(const char *name){
    _name = name;
}

std::string PLConditionLock::name(){
    return _name;
}

int PLConditionLock::condition(){
    return _condition_value;
}

void PLConditionLock::lock(){
    _condition->lock();
}

bool PLConditionLock::tryLock(){
    return _condition->tryLock();
}

bool PLConditionLock::lockBeforeDate(PLDate *limit){
    return _condition->lockBeforeDate(limit);
}

void PLConditionLock::lockWhenCondition(int value){
    _condition->lock();
    while (_condition_value != value) {
        _condition->wait();
    }
}

bool PLConditionLock::tryLockWhenCondition(int value){
    if (_condition->tryLock()) {
        if (_condition_value == value) {
            return true;
        }else{
            _condition->unlock();
        }
    }
    return false;
}

bool PLConditionLock::lockWhenConditionAndBeforeDate(int value, PLDate *limitDate){
    if (_condition->lockBeforeDate(limitDate) == false) {
        return false;
    }
    
    if (_condition_value == value) {
        return true;
    }
    
    while (_condition->waitUntilDate(limitDate)) {
        if (_condition_value == value) {
            return true;
        }
    }
    _condition->unlock();
    return false;
}

void PLConditionLock::unlock(){
    _condition->unlock();
}

void PLConditionLock::unlockWithCondition(int value){
    _condition_value = value;
    _condition->broadcast();
    _condition->unlock();
}

PLRecursiveLock *pl_global_lock;

PLFOUNDATON_NAMESPACE_END





