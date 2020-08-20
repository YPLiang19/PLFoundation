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
private:
    PLTimeInterval _secondsSinceRef;
public:
    static PLDate date();
    static PLDate dateWithTimeIntervalSinceNow(PLTimeInterval secsToBeAdded);
    static PLDate dateWithTimeIntervalSinceReferenceDate(PLTimeInterval secsToBeAdded);
    static PLDate dateWithTimeIntervalSince1970(PLTimeInterval secsToBeAdded);
    static PLDate dateWithTimeInterval(PLTimeInterval secsToBeAdded, PLDate &sinceDate);
    
    static PLDate distantFuture();
    static PLDate distantPast();
    
    PLDate(PLTimeInterval secondsSinceRef = PLPrivateTimeNow()) : _secondsSinceRef(secondsSinceRef){};
    virtual ~PLDate();
    
    PLDate dateByAddingTimeInterval(PLTimeInterval secsToBeAdded);
    
    PLTimeInterval timeIntervalSinceDate(PLDate &anotherDate);
    PLTimeInterval timeIntervalSinceNow();
    PLTimeInterval timeIntervalSince1970();
    PLTimeInterval timeIntervalSinceReferenceDate();
};

PLFOUNDATON_NAMESPACE_END

#endif /* PLDate_hpp */
