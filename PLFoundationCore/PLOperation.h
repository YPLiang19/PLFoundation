//
//  PLOperation.hpp
//  PLFoundation
//
//  Created by yongpengliang on 2020/8/15.
//  Copyright © 2020 yongpengliang. All rights reserved.
//

#ifndef PLOperation_hpp
#define PLOperation_hpp

#include <memory>
#include "PLFoundationCoreMacro.h"
#include "PLObject.h"
#include <list>

typedef int PLOperationQueuePriority;

PLFOUNDATON_NAMESPACE_BEGIN

class PLOperation : public PLObject{
public:
    void addDependency(std::shared_ptr<PLOperation> &op);
    void cancel();
    std::function<void(void)> completionBlock();
    void setCompletionBlock(std::function<void(void)> &completionBlock);
    std::list<std::shared_ptr<PLOperation>>& dependencies();
    bool isCancelled();
    bool isConcurrent();
    bool isExecuting();
    bool isFinished();
    bool isReady();
    PLOperationQueuePriority queuePriority();
    void setQueuePriority(PLOperationQueuePriority priority);
    void start();
    void main();
    void waitUntilFinished();
};



PLFOUNDATON_NAMESPACE_END



#endif /* PLOperation_hpp */
