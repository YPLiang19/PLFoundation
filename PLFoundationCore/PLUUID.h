//
//  PLUUID.hpp
//  PLFoundation
//
//  Created by yongpengliang on 2020/8/20.
//  Copyright © 2020 yongpengliang. All rights reserved.
//

#ifndef PLUUID_hpp
#define PLUUID_hpp


#include "PLFoundationCoreMacro.h"
#include "PLObject.h"


typedef uint8_t uuid_t[16];

PLFOUNDATON_NAMESPACE_BEGIN


class PLUUID : public PLObject {
private:
    uuid_t _uuid;
public:
    static PLUUID UUID();
    PLUUID(std::string str);
    PLUUID(uuid_t bytes);
    std::string UUIDString();
    void getUUIDBytes(uuid_t btyes);
};

PLFOUNDATON_NAMESPACE_END

#endif /* PLUUID_hpp */
