//
//  PLDate.cpp
//  PLFoundation
//
//  Created by yongpengliang on 2020/8/7.
//  Copyright © 2020 yongpengliang. All rights reserved.
//

#include "PLDate.h"
#include <iostream>
#include <sys/time.h>

PLFOUNDATON_NAMESPACE_BEGIN

#define DISTANT_FUTURE    63113990400.0
#define DISTANT_PAST    -63113817600.0

static PLDate *s_distantPast = nullptr;
static PLDate *s_distantFuture = nullptr;

PLDate PLDate::date(){
    return PLDate();
}

PLDate PLDate::dateWithTimeIntervalSinceNow(PLTimeInterval secsToBeAdded){
    return PLDate(PLPrivateTimeNow() + secsToBeAdded);
}

PLDate PLDate::dateWithTimeIntervalSinceReferenceDate(PLTimeInterval secsToBeAdded){
    return PLDate(secsToBeAdded);
}

PLDate PLDate::dateWithTimeIntervalSince1970(PLTimeInterval secsToBeAdded){
    return PLDate(PLTimeIntervalSince1970 + PLPrivateTimeNow());
}

PLDate PLDate::dateWithTimeInterval(PLTimeInterval secsToBeAdded, PLDate &sinceDate){
    return PLDate(sinceDate.timeIntervalSinceReferenceDate() + secsToBeAdded);
}

PLDate PLDate::distantPast(){
    if (s_distantPast == nullptr) {
        s_distantPast = new PLDate(DISTANT_PAST);
    }
    return *s_distantPast;
}

PLDate PLDate::distantFuture(){
    if (s_distantFuture == nullptr) {
        s_distantFuture = new PLDate(DISTANT_FUTURE);
    }
    return *s_distantFuture;
}

PLDate::~PLDate(){
    
}

PLDate PLDate::dateByAddingTimeInterval(PLTimeInterval secsToBeAdded){
    return PLDate(_secondsSinceRef + secsToBeAdded);
}

PLTimeInterval PLDate::timeIntervalSinceDate(PLDate &anotherDate){
    return _secondsSinceRef - anotherDate.timeIntervalSinceReferenceDate();
}

PLTimeInterval PLDate::timeIntervalSinceNow(){
    return _secondsSinceRef - PLPrivateTimeNow();
}

PLTimeInterval PLDate::timeIntervalSince1970(){
    return _secondsSinceRef + PLTimeIntervalSince1970;
}

PLTimeInterval PLDate::timeIntervalSinceReferenceDate(){
    return _secondsSinceRef;
}

PLFOUNDATON_NAMESPACE_END
