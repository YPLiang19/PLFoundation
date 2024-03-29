//
//  PLObject.hpp
//  PLFoundation
//
//  Created by yongpengliang on 2020/8/14.
//  Copyright © 2020 yongpengliang. All rights reserved.
//

#ifndef PLObject_hpp
#define PLObject_hpp

#include <string>
#include <unordered_map>
#include <unordered_set>
#include "PLFoundationCoreMacro.h"


PLFOUNDATON_NAMESPACE_BEGIN

class PLKVOInfo;
class PLRecursiveLock;

class PLObject{
private:
    PLKVOInfo *_observationInfo = nullptr;
    
public:
    virtual ~PLObject();
    
    virtual void observeValueForKeyOfObjectWillChange(std::string key, PLObject *anObject){}
    virtual void observeValueForKeyOfObjectDidChange(std::string key, PLObject *anObject){}
    
    virtual void addObserverForKey(PLObject *anObserver, std::string key);
    virtual void removeObserverForKey(PLObject *anObserver, std::string key);
    
    virtual void willChangeValueForKey(std::string key);
    virtual void didChangeValueForKey(std::string key);
    
};



class PLKVOInfo : public PLObject {
private:
    PLObject *_instance = nullptr; //todo weak
    PLRecursiveLock *_lock = nullptr;
    std::unordered_map<std::string, std::unordered_set<PLObject *>> paths;
public:
    PLKVOInfo(PLObject *obj);
    ~PLKVOInfo();
    void addObserverForKey(PLObject *anObserver, std::string key);
    void removeObserverForKey(PLObject *anObserver, std::string key);
    
    void notifyWillChangeForKey(std::string key);
    void notifyDidChangeForKey(std::string key);
    
    
};

PLFOUNDATON_NAMESPACE_END


#endif /* PLObject_hpp */
