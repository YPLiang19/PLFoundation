//
//  PLObject.cpp
//  PLFoundation
//
//  Created by yongpengliang on 2020/8/14.
//  Copyright © 2020 yongpengliang. All rights reserved.
//

#include "PLObject.h"
#include "PLLock.h"

PLFOUNDATON_NAMESPACE_BEGIN

#pragma mark - PLObject

PLObject::~PLObject(){
    if (_observationInfo) {
        delete _observationInfo;
    }
}

void PLObject::addObserverForKey(PLObject *anObserver, std::string key){
    if (anObserver == nullptr || key.empty()) {
        return;
    }
    if (_observationInfo == nullptr) {
        _observationInfo = new PLKVOInfo(this);
    }
    _observationInfo->addObserverForKey(anObserver, key);
}

void PLObject::removeObserverForKey(PLObject *anObserver, std::string key){
    if (anObserver == nullptr || key.empty()) {
        return;
    }
    if (_observationInfo == nullptr) {
        return;
    }
    _observationInfo->removeObserverForKey(anObserver, key);
}

void PLObject::willChangeValueForKey(std::string key){
    if (_observationInfo == nullptr) {
        return;
    }
    _observationInfo->notifyWillChangeForKey(key);
}

void PLObject::didChangeValueForKey(std::string key){
    if (_observationInfo == nullptr) {
        return;
    }
    _observationInfo->notifyDidChangeForKey(key);
}

#pragma mark - PLKVOInfo

PLKVOInfo::PLKVOInfo(PLObject *obj) : _instance(obj){
    _lock = new PLRecursiveLock();
}

PLKVOInfo::~PLKVOInfo(){
    if (_lock) {
        delete _lock;
    }
}

void PLKVOInfo::addObserverForKey(PLObject *anObserver, std::string key){
    _lock->lock();
    auto iter = paths.find(key);
    if (iter == paths.end()) {
        std::unordered_set<PLObject *> set;
        paths.insert(std::pair<std::string, std::unordered_set<PLObject *>>(key, set));
    }
    iter = paths.find(key);
    (*iter).second.insert(anObserver);
    _lock->unlock();
}

void PLKVOInfo::removeObserverForKey(PLObject *anObserver, std::string key){
    _lock->lock();
    auto iter = paths.find(key);
    if (iter != paths.end()) {
        (*iter).second.erase(anObserver);
    }
    _lock->unlock();
    
}

void PLKVOInfo::notifyWillChangeForKey(std::string key){
    _lock->lock();
    auto mapIter = paths.find(key);
    if (mapIter != paths.end()) {
        auto set = (*mapIter).second;
        for (auto setIter = set.begin(); setIter != set.end() ; setIter++) {
            (*setIter)->observeValueForKeyOfObjectChange(key, _instance, PLKeyValueObservingTypeWillChange);
        }
    }
    _lock->unlock();
}

void PLKVOInfo::notifyDidChangeForKey(std::string key){
    _lock->lock();
    auto mapIter = paths.find(key);
    if (mapIter != paths.end()) {
        auto set = (*mapIter).second;
        for (auto setIter = set.begin(); setIter != set.end() ; setIter++) {
            (*setIter)->observeValueForKeyOfObjectChange(key, _instance, PLKeyValueObservingTypeDidChange);
        }
    }
    _lock->unlock();
}

PLFOUNDATON_NAMESPACE_END
