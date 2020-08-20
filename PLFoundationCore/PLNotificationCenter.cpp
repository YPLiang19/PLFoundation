//
//  PLNotificationCenter.cpp
//  PLFoundation
//
//  Created by yongpengliang on 2020/8/19.
//  Copyright © 2020 yongpengliang. All rights reserved.
//

#include "PLNotificationCenter.h"
#include "PLBlockOperation.h"

PLFOUNDATON_NAMESPACE_BEGIN

using namespace std;

static PLNotificationCenter *s_defaultCenter;


class _NotificationObserver : public PLObject {
private:
    shared_ptr<PLOperationQueue> _queue;
    std::function<void(PLNotification)> _block;
public:
    _NotificationObserver(shared_ptr<PLOperationQueue> queue, std::function<void(PLNotification)> block) : _queue(queue), _block(block){}
    
    void test(){}
    
    void didReceiveNotification(PLNotification notification){
        std::function<void(PLNotification)> block = _block;
        shared_ptr<PLBlockOperation> op = PLBlockOperation::blockOperation([block, notification](){
            if (block) {
                block(notification);
            }
        });
        op->waitUntilFinished();
    }
};


PLNotificationCenter* PLNotificationCenter::defaultCenter(){
    static pl_dispatch_once_t onceToken;
    pl_dispatch_once(&onceToken, [](){
        s_defaultCenter = new PLNotificationCenter();
    });
    return s_defaultCenter;
}


PLNotificationCenter::PLNotificationCenter(){
    pthread_rwlock_init(&_lock, nullptr);
}


PLNotificationCenter::~PLNotificationCenter(){
    pthread_rwlock_destroy(&_lock);
}


void PLNotificationCenter::addObserver(std::string name, void *object, shared_ptr<PLOperationQueue> queue, std::function<void (PLNotification)> block){
    _NotificationObserver *observer = new _NotificationObserver(queue, block);
    this->addObserver(observer, &_NotificationObserver::didReceiveNotification, name, object);
}

void PLNotificationCenter::removeObserver(void *observer){
    this->removeObserver(observer, "", nullptr);
    
}

void PLNotificationCenter::removeObserver(void *observer, std::string name, void *object){
    pthread_rwlock_wrlock(&_lock);
    _observerList.remove_if([observer, &name, object](NotificationObserver &notificationObserver){
        return  notificationObserver.mObserver == observer && notificationObserver.mKey.onRemoveEqaulTo(name, object);
    });
    pthread_rwlock_unlock(&_lock);
}

void PLNotificationCenter::postNotification(PLNotification notification){
    pthread_rwlock_rdlock(&_lock);
    try {
        for (auto i = _observerList.begin(); i != _observerList.end(); i++) {
            NotificationObserver &notificationObserver = *i;
            if (notificationObserver.mKey.onNotificationEqaulTo(notification.name, notification.object)) {
                if (notificationObserver.mInvoke) notificationObserver.mInvoke(notification);
            }
        }
    } catch (...) {
        fprintf(stderr, "PLNotificationCenter::postNotification err");
    }
    pthread_rwlock_unlock(&_lock);
    
}

void PLNotificationCenter::postNotification(std::string name, void *object){
    this->postNotification(name, object, nullptr);
}

void PLNotificationCenter::postNotification(std::string name, void *object, std::unordered_map<void *, void *> *info){
    PLNotification notification(name, object, info);
    this->postNotification(notification);
}



PLFOUNDATON_NAMESPACE_END
