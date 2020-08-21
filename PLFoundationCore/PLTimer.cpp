//
//  PLTimer.cpp
//  PLFoundation
//
//  Created by yongpengliang on 2020/8/20.
//  Copyright © 2020 yongpengliang. All rights reserved.
//

#include "PLTimer.h"

PLFOUNDATON_NAMESPACE_BEGIN

void PLTimer::init(PLTimeInterval interval, void *userInfo, bool repeats){
    if (interval <= 0.0) {
        interval = 0.0001;
    }
    _fireDate = PLDate(_interval);
    _invalidated = false;
    _userInfo = userInfo;
    if (repeats) {
        _repeats = true;
        _interval = interval;
    }else{
        _repeats = false;
        _interval = 0.0;
    }
}

PLTimer::~PLTimer(){
    if (_invalidated == false) {
        invalidate();
    }
}

void PLTimer::invalidate(){
    _invalidated = true;
}

bool PLTimer::isValid(){
    return !_invalidated;
}

PLDate PLTimer::fireDate(){
    return _fireDate;
}

void PLTimer::setFireDate(PLDate &fireDate){
    _fireDate = fireDate;
}

PLTimeInterval PLTimer::timeInterval(){
    return _interval;
}

void * PLTimer::userInfo(){
    return _userInfo;
}

PLComparisonResult PLTimer::compare(std::shared_ptr<PLTimer> anotherTimer){
    if (this == anotherTimer.get()) {
        return PLOrderedSame;
    }
    return _fireDate.compare(anotherTimer->_fireDate);
}

void PLTimer::fire(){
    if (_block) {
        try {
            _block(shared_from_this());
        } catch (...) {
            fprintf(stderr, "PLTimer::fire err");
        }
        if (_repeats == false) {
            invalidate();
        }
    }else{
        invalidate();
    }
    
}

PLFOUNDATON_NAMESPACE_END
