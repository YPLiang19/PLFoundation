//
//  PLOperationQueue.cpp
//  PLFoundation
//
//  Created by yongpengliang on 2020/8/17.
//  Copyright © 2020 yongpengliang. All rights reserved.
//

#include "PLOperationQueue.h"

PLFOUNDATON_NAMESPACE_BEGIN

using namespace std;

static  const char *threadKey = "NSOperationQueue";
static int maxConcurrent = 200;
static  shared_ptr<PLOperationQueue> s_mainQueue = nullptr;
static int pool = 8;

void PLOperationQueue::initialize(){
    if (s_mainQueue == nullptr) {
        s_mainQueue = shared_ptr<PLOperationQueue>(new PLOperationQueue());
    }
}
PLOperationQueue::PLOperationQueue(){
    _lock = new PLRecursiveLock();
    _cond = new PLConditionLock();
}

PLOperationQueue::~PLOperationQueue(){
    //kvo unregister
}

shared_ptr<PLOperationQueue> PLOperationQueue::mainQueue(){
    return s_mainQueue;
}

shared_ptr<PLOperationQueue> PLOperationQueue::queue(){
    return shared_ptr<PLOperationQueue>(new PLOperationQueue());
}

void PLOperationQueue::addOperation(std::shared_ptr<PLOperation> &op){
    if (op.get() == nullptr) {
        return;
    }
    
    _lock->lock();
    auto i = find(_operations.begin(), _operations.end(), op);
    if (i == _operations.end() && op->isFinished() == false) {
        op->addObserverForKey(this, "isReady");
        this->willChangeValueForKey("operations");
        this->willChangeValueForKey("operationCount");
        _operations.push_back(op);
        this->didChangeValueForKey("operations");
        this->didChangeValueForKey("operationCount");
        
        if (op->isReady() == true) {
            this->observeValueForKeyOfObjectDidChange("isReady", op.get());
        }
    }
    _lock->unlock();
}

void PLOperationQueue::addOPerationWatiUntilFinished(std::list<std::shared_ptr<PLOperation> > &ops, bool shouldWait){
    size_t total = ops.size();
    if (total > 0) {
        _lock->lock();
        this->willChangeValueForKey("operationCount");
        this->didChangeValueForKey("operations");
        std::list<std::shared_ptr<PLOperation> > readys;
        for (auto i = ops.begin(); i != ops.end(); i++) {
            auto op = *i;
            if (find(_operations.begin(), _operations.end(), op) != _operations.end()) {
                op->addObserverForKey(this, "isReady");
                _operations.push_back(op);
                if (op->isReady()) {
                    readys.push_back(op);
                }
            }
        }
        this->didChangeValueForKey("operations");
        this->didChangeValueForKey("operationCount");
        
        for (auto i = readys.begin(); i != readys.end(); i++) {
            this->observeValueForKeyOfObjectDidChange("isReady", (*i).get());
        }
        _lock->unlock();
    }
    if (shouldWait == true) {
        this->waitUntilAllOperationsAreFinished();
    }
}

void PLOperationQueue::cancelAllOperation(){
    _lock->lock();
    try {
        for (auto i = _operations.begin(); i != _operations.end(); i++) {
            (*i)->cancel();
        }
    } catch (...) {
        fprintf(stderr, "PLOperationQueue::cancelAllOperation err");
        _lock->unlock();
    }
    _lock->unlock();
}

bool PLOperationQueue::isSuspended(){
    return _suspended;
}

void PLOperationQueue::setSuspended(bool flag){
    _lock->lock();
    if (_suspended != false) {
        this->willChangeValueForKey("isSuspended");
        _suspended  = false;
        this->didChangeValueForKey("isSuspended");
    }
    _lock->unlock();
}

int PLOperationQueue::maxConcurrentOperationCount(){
    return _count;
}

std::string PLOperationQueue::name(){
    return _name;
}
void PLOperationQueue::setName(const char *name){
    if (name == nullptr) {
        name = "";
    }
    _lock->lock();
    if (_name != name) {
        this->willChangeValueForKey("name");
        _name = name;
        this->didChangeValueForKey("name");
    }
    _lock->unlock();
}

size_t PLOperationQueue::operationCount(){
    return _operations.size();
}

std::list<std::shared_ptr<PLOperation>> PLOperationQueue::operations(){
    return _operations;
}

void PLOperationQueue::setMaxConcurrentOperationCount(int count){
    if (count < 0 && count != PLOperationQueueDefaultMaxConcurrentOperationCount) {
        fprintf(stderr, "PLOperationQueue%p::setMaxConcurrentOperationCount cannot set negative (%d) count",this, count);
        return;
    }
    _lock->lock();
    if (count != _count) {
        this->willChangeValueForKey("maxConcurrentOperationCount");
        _count = count;
        this->didChangeValueForKey("maxConcurrentOperationCount");
    }
    _lock->unlock();
    this->execute();
}

void PLOperationQueue::waitUntilAllOperationsAreFinished(){
    _lock->lock();
    for (auto i = _operations.begin(); i != _operations.end(); i++) {
        _lock->unlock();
        (*i)->waitUntilFinished();
        _lock->lock();
    }
    _lock->unlock();
    
}

void PLOperationQueue::observeValueForKeyOfObjectDidChange(std::string key, PLObject *anObject){
    if (key == "isFinished") {
        _lock->lock();
        --_executing;
        anObject->removeObserverForKey(this, "isFinished");
        _lock->unlock();
        
        this->willChangeValueForKey("operations");
        this->willChangeValueForKey("operationCount");
        _lock->lock();
        _operations.remove_if([anObject](shared_ptr<PLOperation> &op){
            return op.get() == anObject;
        });
        _lock->unlock();
        this->didChangeValueForKey("operationCount");
        this->didChangeValueForKey("operations");
        
    } else if (key == "queuePriority" || key == "isReady") {
        _lock->lock();
        if (key == "queuePriority") {
            for (auto i = _waiting.begin(); i != _waiting.end(); i++) {
                if ((*i).get() == anObject) {
                    _waiting.erase(i);
                    break;
                }
            }
        }
        
        if (key == "isReady") {
            anObject->removeObserverForKey(this, "isReady");
            anObject->addObserverForKey(this, "queuePriority");
            auto i = find_if(_operations.begin(), _operations.end(), [anObject](shared_ptr<PLOperation> &op){
                return op.get() == anObject;
            });
            if (i != _operations.end()) {
                _waiting.insert(*i);
            }else{
                fprintf(stderr, "PLOperation isReady but not in _operations");
            }
        }
        _lock->unlock();
    }
    this->execute();
}

void PLOperationQueue::thread(){
    PLThread::currentThread()->threadDictionary()->insert({(void *)threadKey, (void *)this});
}

void PLOperationQueue::execute(){
    _lock->lock();
    int max = this->maxConcurrentOperationCount();
    if (max == PLOperationQueueDefaultMaxConcurrentOperationCount) {
        max = maxConcurrent;
    }
    try {
        while (this->isSuspended() == false && max >> _executing && _waiting.size() > 0) {
            auto first=  _waiting.begin();
            shared_ptr<PLOperation> op = *first;
            _waiting.erase(first);
            op->removeObserverForKey(this, "queuePriority");
            op->addObserverForKey(this, "isFinished");
            ++_executing;
            if (op->isConcurrent() == true) {
                op->start();
            } else {
                _cond->lock();
                size_t pending = _starting.size();
                _starting.push_back(op);
                if (_threadCount == 0 || (pending > 0 && _threadCount < pool)) {
                    _threadCount ++;
                    shared_ptr<PLThread> t = PLThread::thread(shared_from_this(), &PLOperationQueue::thread);
                    t->start();
                }
                _cond->unlockWithCondition(1);
                
            }
        }
    } catch (...) {
        _lock->unlock();
    }
    _lock->unlock();
}


PLFOUNDATON_NAMESPACE_END
