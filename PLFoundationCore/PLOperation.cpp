//
//  PLOperation.cpp
//  PLFoundation
//
//  Created by yongpengliang on 2020/8/15.
//  Copyright © 2020 yongpengliang. All rights reserved.
//

#include "PLOperation.h"


PLFOUNDATON_NAMESPACE_BEGIN

using namespace std;

PLOperation::PLOperation(){
    _lock = new PLRecursiveLock();
    _cond = new PLConditionLock();
    this->addObserverForKey(this, "isFinished");
}

PLOperation::~PLOperation(){
    _lock->lock();
    this->removeObserverForKey(this, "isFinished");
    for (auto i = _dependencies.begin(); i != _dependencies.end(); i++) {
        (*i)->removeObserverForKey(this, "isFinished");
    }
    _lock->unlock();
    
    delete _lock;
    delete _cond;
}


void PLOperation::addDependency(std::shared_ptr<PLOperation> &op){
    if (op.get() == this) {
        fprintf(stderr, "PLOperation%p attempt to add dependency on self", this);
        return;
    }
    _lock->lock();
    try {
        auto iter = find(_dependencies.begin(), _dependencies.begin(), op);
        if (iter != _dependencies.end()) {
            this->willChangeValueForKey("dependencies");
            _dependencies.push_back(op);
            if (op->isFinished() == false && op->isCancelled() == false && op->isExecuting() == false && op->isFinished()) {
                op->addObserverForKey(this, "isFinished");
                if (_ready == true) {
                    this->willChangeValueForKey("isReady");
                    _ready = false;
                    this->didChangeValueForKey("isReady");
                }
            }
            this->didChangeValueForKey("dependencies");
        }
    } catch (...) {
        _lock->unlock();
    }
    _lock->unlock();
    
}

void PLOperation::removeDependency(std::shared_ptr<PLOperation> &op){
    _lock->lock();
    auto iter = find(_dependencies.begin(), _dependencies.end(), op);
    if (iter != _dependencies.end()) {
        op->removeObserverForKey(this, "isFinished");
        this->willChangeValueForKey("dependencies");
        _dependencies.remove(op);
        if (_ready == false) {
            this->observeValueForKeyOfObjectDidChange("isFinished", op.get());
        }
        this->didChangeValueForKey("dependencies");
    }
    _lock->unlock();
}

std::list<std::shared_ptr<PLOperation>> PLOperation::dependencies(){
    return _dependencies;
}

bool PLOperation::isCancelled(){
    return _cancelled;
}

void PLOperation::cancel(){
    if (_cancelled == false && _finished == false) {
        _lock->lock();
        try {
            if (_cancelled == false && _finished == false) {
                this->willChangeValueForKey("isCancelled");
                if (_ready == false) {
                    this->willChangeValueForKey("isReady");
                    _ready = true;
                    this->didChangeValueForKey("isReady");
                }
                this->didChangeValueForKey("isCancelled");
            }
        } catch (...) {
            _lock->unlock();
        }
        _lock->unlock();
    }
}

bool PLOperation::isExecuting(){
    return _executing;
}

bool PLOperation::isFinished(){
    return _finished;
}

bool PLOperation::isConcurrent(){
    return _concurrent;
}

bool PLOperation::isReady(){
    return _ready;
}

void PLOperation::main(){
    return;
}

void PLOperation::observeValueForKeyOfObjectDidChange(std::string path, PLObject *anObject){
    _lock->lock();
    anObject->removeObserverForKey(this, "isFinished");
    
    if (anObject == this) {
        _cond->lock();
        _cond->unlockWithCondition(1);
        _lock->unlock();
        return;
    }
    
    if (_ready == false) {
        auto iter = _dependencies.begin();
        for (; iter != _dependencies.end(); iter++) {
            if ((*iter)->isFinished() == false) {
                break;
            }
        }
        
        if (iter ==_dependencies.end()) {
            this->willChangeValueForKey("isReady");
            _ready = true;
            this->didChangeValueForKey("isReady");
        }
    }
    _lock->unlock();
}

PLOperationQueuePriority PLOperation::queuePriority(){
    return _priority;
}

void PLOperation::setQueuePriority(PLOperationQueuePriority pri){
    if (pri <= PLOperationQueuePriorityVeryLow)
      pri = PLOperationQueuePriorityVeryLow;
    else if (pri <= PLOperationQueuePriorityLow)
      pri = PLOperationQueuePriorityLow;
    else if (pri < PLOperationQueuePriorityHigh)
      pri = PLOperationQueuePriorityNormal;
    else if (pri < PLOperationQueuePriorityVeryHigh)
      pri = PLOperationQueuePriorityHigh;
    else
      pri = PLOperationQueuePriorityVeryHigh;

    if (pri != _priority) {
          _lock->lock();
        if (pri != _priority) {
            try {
                this->willChangeValueForKey("queuePriority");
                _priority = pri;
                this->didChangeValueForKey("queuePriority");
            } catch (...) {
                _lock->unlock();
            }
            _lock->unlock();
        }
     }
    
}

function<void(void)> PLOperation::completionBlock(){
    return _completionBlock;
}

void PLOperation::setCompletionBlock(std::function<void ()> &completionBlock){
    _completionBlock = completionBlock;
}

void PLOperation::waitUntilFinished(){
    _cond->lockWhenCondition(1);
    _cond->unlockWithCondition(1);
}


void PLOperation::start(){
    _lock->lock();
    try {
        if (_executing == true) {
            _lock->unlock();
            return;
        }
        if (_finished == true) {
            _lock->unlock();
            return;
        }
        if (_ready == false) {
            _lock->unlock();
            return;
        }
        this->willChangeValueForKey("isExecuting");
        _executing = true;
        this->didChangeValueForKey("isExecuting");
        
    } catch (...) {
        _lock->unlock();
    }
    
    _lock->unlock();
    
    try {
        if (_cancelled == false) {
            this->main();
        }
    } catch (...) {
        fprintf(stderr, "PLOperation%p::start error", this);
    }
    
    this->finish();
}

void PLOperation::finish(){
    _lock->lock();
    if (_finished == false) {
        if (_executing == true) {
            this->willChangeValueForKey("isExecuting");
            this->willChangeValueForKey("isFinished");
            _executing = false;
            _finished = true;
            this->didChangeValueForKey("isFinished");
            this->didChangeValueForKey("isExecuting");
        }else{
            this->willChangeValueForKey("isFinished");
            _finished = true;
            this->didChangeValueForKey("isFinished");
            
        }
        if (_completionBlock) {
            _completionBlock();
        }
    }
    _lock->unlock();
}


PLFOUNDATON_NAMESPACE_END

