//
//  PLInvocationOperation.hpp
//  PLFoundation
//
//  Created by yongpengliang on 2020/8/19.
//  Copyright © 2020 yongpengliang. All rights reserved.
//

#ifndef PLInvocationOperation_hpp
#define PLInvocationOperation_hpp

#include "PLOperation.h"

PLFOUNDATON_NAMESPACE_BEGIN

class PLInvocationOperation : public PLOperation {
private:
    std::function<void(void)> _block;
    
    template<typename Object, typename Method>
    PLInvocationOperation(std::shared_ptr<Object> objectPtr, Method method) : PLOperation(){
        _block = [objectPtr, method](){
            if (objectPtr) (objectPtr.get()->*method)();
        };
    }
    
    template<typename Object, typename Method>
    PLInvocationOperation(std::weak_ptr<Object> objectPtr, Method method) : PLOperation(){
        _block = [objectPtr, method](){
            if (!objectPtr.expired()) (objectPtr.lock().get()->*method)();
        };
    }
    
    template<typename Object, typename Method>
    PLInvocationOperation(Object *objectPtr, Method method) : PLOperation(){
        _block = [objectPtr, method](){
            if (objectPtr) (objectPtr->*method)();
        };
    }
    
public:
    template<typename ObjectPtr, typename Method>
    static std::shared_ptr<PLInvocationOperation> blockOperation(ObjectPtr objectPtr, Method method){
        std::shared_ptr<PLInvocationOperation> tp(new PLInvocationOperation(objectPtr, method));
        return tp;
    }
    
    void main(){ if (_block) _block(); }
};

PLFOUNDATON_NAMESPACE_END

#endif /* PLInvocationOperation_hpp */
