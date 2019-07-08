//
//  CrossBase.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include "CrossBase.hpp"

#include <android/log.h>
#include <CrossPLFactory.hpp>
#include <list>

/***********************************************/
/***** static variables initialize *************/
/***********************************************/


/***********************************************/
/***** static function implement ***************/
/***********************************************/
static auto gCreateCppObjFuncList = std::list<int64_t(*)(const char*)>();
static auto gDestroyCppObjFuncList = std::list<int(*)(const char*,int64_t)>();

void RegCreateCppObjFunc(int64_t(*func)(const char*))
{
    __android_log_print(ANDROID_LOG_DEBUG, "crosspl", "%s", __PRETTY_FUNCTION__);
    gCreateCppObjFuncList.push_back(func);
}
void RegDestroyCppObjFunc(int(*func)(const char*,int64_t))
{
    __android_log_print(ANDROID_LOG_DEBUG, "crosspl", "%s", __PRETTY_FUNCTION__);
    gDestroyCppObjFuncList.push_back(func);
}

int64_t CrossBase::CreateNativeObject(const char* javaClassName)
{
    __android_log_print(ANDROID_LOG_DEBUG, "crosspl", "%s", __PRETTY_FUNCTION__);

    for(auto func: gCreateCppObjFuncList) {
        auto cppHandle = func(javaClassName);
        if(cppHandle != 0) { // success
            return cppHandle;
        }
    }

    // need return before here.
    throw std::runtime_error("CrossPL: Failed to create cpp object.");
}

void CrossBase::DestroyNativeObject(const char* javaClassName, int64_t nativeHandle)
{
    __android_log_print(ANDROID_LOG_DEBUG, "crosspl", "%s", __PRETTY_FUNCTION__);

    for(auto func: gDestroyCppObjFuncList) {
        auto cppHandle = func(javaClassName, nativeHandle);
        if(cppHandle == 0) { // success
            return;
        }
    }

    // need return before here.
    throw std::runtime_error("CrossPL: Failed to destroy cpp object.");
}


/***********************************************/
/***** class public function implement  ********/
/***********************************************/
uint64_t CrossBase::getPlatformHandle() const
{
    return 0;
}


/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/
