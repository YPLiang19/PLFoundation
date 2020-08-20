//
//  PLNotification.hpp
//  PLFoundation
//
//  Created by yongpengliang on 2020/8/19.
//  Copyright © 2020 yongpengliang. All rights reserved.
//

#ifndef PLNotification_hpp
#define PLNotification_hpp

#include "PLFoundationCoreMacro.h"
#include "PLObject.h"
#include <unordered_map>

PLFOUNDATON_NAMESPACE_BEGIN

struct PLNotification : public PLObject{
    std::string name;
    void *object;
    std::unordered_map<void *, void *> *info;
    
    PLNotification(std::string name, void *object, std::unordered_map<void *, void *> *info) : name(name), object(object), info(info){};
    PLNotification(std::string name, void *object) : PLNotification(name, object, nullptr){}
    
};

PLFOUNDATON_NAMESPACE_END

#endif /* PLNotification_hpp */
