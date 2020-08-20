//
//  PLTimer.hpp
//  PLFoundation
//
//  Created by yongpengliang on 2020/8/20.
//  Copyright © 2020 yongpengliang. All rights reserved.
//

#ifndef PLTimer_hpp
#define PLTimer_hpp

#include "PLFoundationCoreMacro.h"
#include "PLObject.h"
#include "PLDate.h"
#include "PLRunLoop.h"

PLFOUNDATON_NAMESPACE_BEGIN

class PLTimer : public PLObject, public std::enable_shared_from_this<PLTimer> {
private:
    PLDate _fireDate;
    bool _invalidated;
    bool _repeats;
    PLTimeInterval _interval;
    void *_userInfo;
    std::function<void(std::shared_ptr<PLTimer>)> _invoke;
    
    template<typename Object, typename Method>
    PLTimer(PLTimeInterval interval, std::shared_ptr<Object> objectPtr, Method method, void *userInfo, bool repeats){
        _invoke = [objectPtr, method](std::shared_ptr<PLTimer> timer){
            if (objectPtr) (objectPtr.get()->*method)(timer);
        };
        init(interval, userInfo, repeats);
    }
    
    template<typename Object, typename Method>
    PLTimer(PLTimeInterval interval, std::weak_ptr<Object> objectPtr, Method method, void *userInfo, bool repeats){
        _invoke = [objectPtr, method](std::shared_ptr<PLTimer> timer){
            if (!objectPtr.expired()) (objectPtr.lock().get()->*method)(timer);
        };
        init(interval, userInfo, repeats);
    }
    
   template<typename Object, typename Method>
   PLTimer(PLTimeInterval interval, Object *objectPtr, Method method, void *userInfo, bool repeats){
       _invoke = [objectPtr, method](std::shared_ptr<PLTimer> timer){
            if (objectPtr) (objectPtr->*method)(timer);
       };
       init(interval, userInfo, repeats);
   }
    
    void init(PLTimeInterval interval, void *userInfo, bool repeats);
    
public:
    template<typename ObjectPtr, typename Method>
    static std::shared_ptr<PLTimer> scheduledTimerWithTimeInterval(PLTimeInterval interval, ObjectPtr objectPtr, Method method, void *userInfo, bool repeats){
        std::shared_ptr<PLTimer> timer(new PLTimer(interval, objectPtr, method, userInfo, repeats));
        PLRunLoop::currentRunLoop()->addTimer(timer);
        return timer;
    }
    
    template<typename ObjectPtr, typename Method>
    static std::shared_ptr<PLTimer> timerWithTimeInterval(PLTimeInterval interval, ObjectPtr objectPtr, Method method, void *userInfo, bool repeats){
         return std::shared_ptr<PLTimer>(new PLTimer(interval, objectPtr, method, userInfo, repeats));
    }
    
    virtual ~PLTimer();
    
    void fire();
    PLDate fireDate();
    void invalidate();
    void *userInfo();
    bool isValid();
    PLTimeInterval timeInterval();
    void setFireDate(PLDate &fireDate);
    
    PLComparisonResult compare(std::shared_ptr<PLTimer> anotherTimer);

};

PLFOUNDATON_NAMESPACE_END

#endif /* PLTimer_hpp */
