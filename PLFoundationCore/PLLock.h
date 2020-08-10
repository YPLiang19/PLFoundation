//
//  PLLock.hpp
//  PLFoundation
//
//  Created by yongpengliang on 2020/8/7.
//  Copyright © 2020 yongpengliang. All rights reserved.
//

#ifndef PLLock_hpp
#define PLLock_hpp
#include "PLFoundationCoreMacro.h"
#include "PLDate.h"
#include <pthread.h>
#include <string>

PLFOUNDATON_NAMESPACE_BEGIN

class PLLocking{
public:
    virtual ~PLLocking(){}
    virtual void lock() = 0;
    virtual void unlock() = 0;
};


class PLLock : public PLLocking{
protected:
    pthread_mutex_t _mutex;
    std::string _name;
public:
    PLLock();
    virtual ~PLLock();
    
    void setName(const char *name);
    std::string name();
    
    void lock();
    bool tryLock();
    
    bool lockBeforeDate(PLDate *limit);
    void unlock();
};


class PLRecursiveLock : public PLLocking{
protected:
    pthread_mutex_t _mutex;
    std::string _name;
public:
    PLRecursiveLock();
    virtual ~PLRecursiveLock();
    
    void setName(const char *name);
    std::string name();
    
    void lock();
    bool tryLock();
    bool lockBeforeDate(PLDate *limit);
    
    void unlock();
};

class PLConditionLock;
class PLCondition : public PLLocking{
    friend PLConditionLock;
protected:
    pthread_cond_t _condition;
    pthread_mutex_t _mutex;
    std::string _name;
public:
    PLCondition();
    ~PLCondition();
    
    void setName(std::string &&name);
    std::string& name();
    
    void lock();
    void unlock();
    
    void wait();
    bool waitUntilDate(PLDate *limit);
    void signal();
    void broadcast();
private:
    bool tryLock();
    bool lockBeforeDate(PLDate *limit);
};

class PLConditionLock : public PLLocking{
protected:
    PLCondition *_condition;
    int _condition_value;
    std::string _name;
public:
    PLConditionLock(int value = 0);
    ~PLConditionLock();
    
    void setName(const char *name);
    std::string name();
    
    int condition();
    
    void lock();
    bool tryLock();
    bool lockBeforeDate(PLDate *limit);
    void lockWhenCondition(int value);
    bool tryLockWhenCondition(int value);
    bool lockWhenConditionAndBeforeDate(int value, PLDate *limitDate);
    
    void unlock();
    void unlockWithCondition(int value);
};

PLFOUNDATON_NAMESPACE_END

#endif /* PLLock_hpp */
