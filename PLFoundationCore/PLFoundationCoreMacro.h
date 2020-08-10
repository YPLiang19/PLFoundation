//
//  PLFoundationCoreMacro.h
//  PLFoundation
//
//  Created by yongpengliang on 2020/8/7.
//  Copyright © 2020 yongpengliang. All rights reserved.
//

#ifndef PLFoundationCoreMacro_h
#define PLFoundationCoreMacro_h

#define  PLFOUNDATON_NAMESPACE_BEGIN  namespace PLFoundation {

#define  PLFOUNDATON_NAMESPACE_END }

typedef long pl_dispatch_once_t;
typedef void (*pl_dispatch_block_t)();
typedef double PLTimeInterval;

inline void pl_dispatch_once(pl_dispatch_once_t *predicate, pl_dispatch_block_t block){
    if (__sync_bool_compare_and_swap(predicate,0,1)) {
        block();
    }
}


#endif /* PLFoundationCoreMacro_h */
