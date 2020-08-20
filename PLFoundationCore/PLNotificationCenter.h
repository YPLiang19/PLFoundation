//
//  PLNotificationCenter.hpp
//  PLFoundation
//
//  Created by yongpengliang on 2020/8/19.
//  Copyright © 2020 yongpengliang. All rights reserved.
//

#ifndef PLNotificationCenter_hpp
#define PLNotificationCenter_hpp

#include "PLFoundationCoreMacro.h"
#include "PLObject.h"
#include "PLOperationQueue.h"
#include "PLNotification.h"
#include <unordered_map>
#include <functional>

PLFOUNDATON_NAMESPACE_BEGIN

struct NotificationObserverKey{
    std::string mName;
    void *mObject = nullptr;
    
    
    bool operator==(NotificationObserverKey &o){
        return mName == o.mName && mObject == o.mObject;
    }
    
    bool onRemoveEqaulTo(std::string name, void *object){
        if (name.empty() && object == nullptr) {
            return true;
        }
        
        if (object == nullptr) {
            return mName == name;;
        }
        
        if (name.empty()) {
            return mObject == object;
        }
        return mName == name  && mObject == object;
    }
    
    bool onNotificationEqaulTo(std::string name, void *object){
        if (mName.empty() && mObject == nullptr) {
            return true;
        }

        if (mObject == nullptr) {
            return mName == name;;
        }

        if (mName.empty()) {
            return mObject == object;
        }
        return mName == name  && mObject == object;
    }
};

struct NotificationObserver {
    void *mObserver;
    NotificationObserverKey mKey;
    std::function<void(PLNotification)> mInvoke;
};


class PLNotificationCenter : PLObject {
private:
    pthread_rwlock_t _lock;
    std::list<NotificationObserver> _observerList;
    
    
public:
    
    PLNotificationCenter();
    virtual ~PLNotificationCenter();
    
    static PLNotificationCenter* defaultCenter();
    
    template<typename Object, typename Method>
    void addObserver(Object *observer, Method method, std::string name, void *object){
        NotificationObserver notificationObserver;
        notificationObserver.mKey.mName = name;
        notificationObserver.mKey.mObject = object;
        notificationObserver.mObserver = object;
        notificationObserver.mInvoke = [observer, method](PLNotification notification){
            if (observer) (observer->*method)(notification);
        };
        pthread_rwlock_wrlock(&_lock);
        _observerList.push_back(notificationObserver);
        pthread_rwlock_unlock(&_lock);
    }
    void addObserver(std::string name, void *object, std::shared_ptr<PLOperationQueue> queue, std::function<void(PLNotification)> block);
    
    void removeObserver(void *observer);
    void removeObserver(void *observer, std::string name, void *object);
    
    void postNotification(PLNotification notification);
    void postNotification(std::string name, void *object);
    void postNotification(std::string name, void *object, std::unordered_map<void *, void *> *info);
    
};


PLFOUNDATON_NAMESPACE_END

#endif /* PLNotificationCenter_hpp */
