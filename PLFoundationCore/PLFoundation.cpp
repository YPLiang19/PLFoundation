//
//  PLFoundation.cpp
//  PLFoundation
//
//  Created by yongpengliang on 2020/8/14.
//  Copyright © 2020 yongpengliang. All rights reserved.
//

#include <PLFoundation.h>
#include "PLLock.h"

PLFOUNDATON_NAMESPACE_BEGIN

//__attribute__((constructor))
void pl_init(){
    pl_global_lock = new PLRecursiveLock();
}

PLFOUNDATON_NAMESPACE_END
