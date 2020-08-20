//
//  PLRunLoop.hpp
//  PLFoundation
//
//  Created by yongpengliang on 2020/8/20.
//  Copyright © 2020 yongpengliang. All rights reserved.
//

#ifndef PLRunLoop_hpp
#define PLRunLoop_hpp

#include "PLFoundationCoreMacro.h"
#include "PLObject.h"


PLFOUNDATON_NAMESPACE_BEGIN
class PLTimer;
class PLRunLoop : public PLObject {
public:
    static PLRunLoop *currentRunLoop();
    void addTimer(std::shared_ptr<PLTimer> timer);
};

PLFOUNDATON_NAMESPACE_END

#endif /* PLRunLoop_hpp */
