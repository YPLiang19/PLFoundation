//
//  PLOperation.hpp
//  PLFoundation
//
//  Created by yongpengliang on 2020/8/15.
//  Copyright © 2020 yongpengliang. All rights reserved.
//

#ifndef PLOperation_hpp
#define PLOperation_hpp

#include <list>
#include <memory>
#include "PLFoundationCoreMacro.h"
#include "PLObject.h"
#include "PLLock.h"

typedef int PLOperationQueuePriority;
enum {
  PLOperationQueuePriorityVeryLow = -8,
  PLOperationQueuePriorityLow = -4,
  PLOperationQueuePriorityNormal = 0,
  PLOperationQueuePriorityHigh = 4,
  PLOperationQueuePriorityVeryHigh = 8
};


PLFOUNDATON_NAMESPACE_BEGIN


class PLOperation : public PLObject, public std::enable_shared_from_this<PLOperation>{
private:
    PLRecursiveLock *_lock;
    PLConditionLock *_cond;
    PLOperationQueuePriority _priority = PLOperationQueuePriorityNormal;
    bool _cancelled = false;
    bool _concurrent = false;
    bool _executing = false;
    bool _finished = false;
    bool _blocked = false;
    bool _ready = true;
    std::list<std::shared_ptr<PLOperation>> _dependencies;
    std::function<void(void)> _completionBlock;
    
    void finish();
    
protected:
    PLOperation();
    
public:
   virtual  ~PLOperation();
    
    virtual void addDependency(std::shared_ptr<PLOperation> &op);
    virtual void removeDependency(std::shared_ptr<PLOperation> &op);
    virtual void cancel();
    virtual std::function<void(void)> completionBlock();
    virtual void setCompletionBlock(std::function<void(void)> &completionBlock);
    virtual std::list<std::shared_ptr<PLOperation>> dependencies();
    virtual bool isCancelled();
    virtual bool isConcurrent();
    virtual bool isExecuting();
    virtual bool isFinished();
    virtual bool isReady();
    virtual PLOperationQueuePriority queuePriority();
    virtual void setQueuePriority(PLOperationQueuePriority priority);
    virtual void start();
    virtual void main();
    virtual void waitUntilFinished();
    
    virtual void observeValueForKeyOfObjectDidChange(std::string path, PLObject *anObject);
    
};



PLFOUNDATON_NAMESPACE_END



#endif /* PLOperation_hpp */
