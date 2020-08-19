//
//  PLBlockOperation.hpp
//  PLFoundation
//
//  Created by yongpengliang on 2020/8/19.
//  Copyright © 2020 yongpengliang. All rights reserved.
//

#ifndef PLBlockOperation_hpp
#define PLBlockOperation_hpp

#include "PLOperation.h"

PLFOUNDATON_NAMESPACE_BEGIN

class PLBlockOperation : public PLOperation {
private:
    std::function<void(void)> _block;
    PLBlockOperation(std::function<void(void)> &block) : PLOperation(), _block(block){}
    
public:
    static std::shared_ptr<PLBlockOperation> blockOperation(std::function<void(void)> block){
        return std::shared_ptr<PLBlockOperation>(new PLBlockOperation(block));
    }
    
    void main(){ if (_block) _block(); }
};

PLFOUNDATON_NAMESPACE_END

#endif /* PLBlockOperation_hpp */
