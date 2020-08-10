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
    virtual void lock() = 0;
    virtual void unlock() = 0;
};


class PLLock : public PLLocking{
protected:
    pthread_mutex_t _mutex;
    std::string _name;
public:
    PLLock();
    void setName(std::string &name);
    std::string& name();
    void lock();
    bool tryLock();
    bool lockBeforeDate(PLDate *limit);
    void unLock();
    virtual ~PLLock();
};


class PLRecursiveLock : public PLLocking{
protected:
    pthread_mutex_t _mutex;
    std::string _name;
public:
    PLRecursiveLock();
    void setName(std::string &name);
    std::string& name();
    void lock();
    bool tryLock();
    bool lockBeforeDate(PLDate *limit);
    void unLock();
    virtual ~PLRecursiveLock();
};


class PLCondition : public PLLocking{
protected:
    pthread_cond_t _condition;
    pthread_mutex_t _mutex;
    std::string _name;
public:
    void wait();
    bool waitUntilDate(PLDate *limit);
    void signal();
    void broadcast();
    void setName(std::string &name);
    std::string name();
};






PLFOUNDATON_NAMESPACE_END

#endif /* PLLock_hpp */
