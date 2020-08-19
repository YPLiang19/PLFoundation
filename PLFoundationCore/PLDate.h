//
//  PLDate.hpp
//  PLFoundation
//
//  Created by yongpengliang on 2020/8/7.
//  Copyright © 2020 yongpengliang. All rights reserved.
//

#ifndef PLDate_hpp
#define PLDate_hpp

#include "PLFoundationCoreMacro.h"
#include "PLObject.h"

PLFOUNDATON_NAMESPACE_BEGIN

class PLDate : public PLObject{
public:
    static PLDate *dateWithTimeIntervalSinceNow(PLTimeInterval secsToBeAdded);
    
    PLTimeInterval timeIntervalSinceNow();
    PLTimeInterval timeIntervalSince1970();
    PLTimeInterval timeIntervalSinceReferenceDate();
};

PLFOUNDATON_NAMESPACE_END

#endif /* PLDate_hpp */
