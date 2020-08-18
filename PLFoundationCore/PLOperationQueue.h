//
//  PLOperationQueue.hpp
//  PLFoundation
//
//  Created by yongpengliang on 2020/8/17.
//  Copyright © 2020 yongpengliang. All rights reserved.
//

#ifndef PLOperationQueue_hpp
#define PLOperationQueue_hpp

#include <list>
#include <memory>
#include <set>
#include "PLFoundationCoreMacro.h"
#include "PLObject.h"
#include "PLLock.h"
#include "PLOperation.h"
#include "PLThread.h"

enum {
   PLOperationQueueDefaultMaxConcurrentOperationCount = -1
};

PLFOUNDATON_NAMESPACE_BEGIN

struct waitingSetCmp{
    bool operator()(const std::shared_ptr<PLOperation> &op1, const std::shared_ptr<PLOperation> &op2) const{
        return op1->queuePriority() >= op2->queuePriority();
    }
};

class PLOperationQueue : public PLObject, public std::enable_shared_from_this<PLOperationQueue>{
private:
    PLRecursiveLock *_lock;
    PLConditionLock *_cond;
    std::list<std::shared_ptr<PLOperation>> _operations;
    std::set<std::shared_ptr<PLOperation>, waitingSetCmp> _waiting;
    std::list<std::shared_ptr<PLOperation>> _starting;
    std::string _name;
    bool _suspended = false;
    int _executing = false;
    int _threadCount = 0;
    int _count = PLOperationQueueDefaultMaxConcurrentOperationCount;
    
    void execute();
    void thread();
    void observeValueForKeyOfObjectDidChange(std::string path, PLObject *anObject);
    
protected:
    PLOperationQueue();
    
public:
    
    static void initialize();//todo 必须在主线程上调用
    static std::shared_ptr<PLOperationQueue> mainQueue();
    static std::shared_ptr<PLOperationQueue> queue();
    virtual ~PLOperationQueue();
    
    void addOperation(std::shared_ptr<PLOperation> &op);
    void addOPerationWatiUntilFinished(std::list<std::shared_ptr<PLOperation>> &ops, bool shouldWait);
    void cancelAllOperation();
    bool isSuspended();
    void setSuspended(bool flag);
    int maxConcurrentOperationCount();
    void setMaxConcurrentOperationCount(int count);
    std::string name();
    void setName(const char *name);
    size_t operationCount();
    std::list<std::shared_ptr<PLOperation>> operations();
    void waitUntilAllOperationsAreFinished();
    
};

PLFOUNDATON_NAMESPACE_END

#endif /* PLOperationQueue_hpp */
