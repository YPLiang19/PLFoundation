//
//  PLFoundationCoreMacro.h
//  PLFoundation
//
//  Created by yongpengliang on 2020/8/7.
//  Copyright © 2020 yongpengliang. All rights reserved.
//

#ifndef PLFoundationCoreMacro_h
#define PLFoundationCoreMacro_h

#include <sys/time.h>


#define  PLFOUNDATON_NAMESPACE_BEGIN  namespace PLFoundation {
#define  PLFOUNDATON_NAMESPACE_END }

#ifdef __cplusplus
#define PL_EXTERN_C_BEGIN  extern "C" {
#define PL_EXTERN_C_END  }
#else
#define PL_EXTERN_C_BEGIN
#define PL_EXTERN_C_END
#endif

PL_EXTERN_C_BEGIN


typedef enum{
    PLOrderedAscending = -1,
    PLOrderedSame,
    PLOrderedDescending
} PLComparisonResult;

typedef long pl_dispatch_once_t;
typedef void (*pl_dispatch_block_t)();
typedef double PLTimeInterval;


const PLTimeInterval PLTimeIntervalSince1970 = 978307200.0;


static inline void pl_dispatch_once(pl_dispatch_once_t *predicate, pl_dispatch_block_t block){
    if (__sync_bool_compare_and_swap(predicate,0,1)) {
        block();
    }
}

static inline PLTimeInterval PLPrivateTimeNow(void) {
  PLTimeInterval t;
  struct timeval tp;

  gettimeofday(&tp, NULL);
  t = (PLTimeInterval)tp.tv_sec - PLTimeIntervalSince1970;
  t += (PLTimeInterval)tp.tv_usec / 1000000.0;
  return t;
}


PL_EXTERN_C_END

#endif /* PLFoundationCoreMacro_h */
