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
static auto gCreateJavaObjFuncList = std::list<int64_t(*)(const char*, int64_t)>();
static auto gDestroyJavaObjFuncList = std::list<int(*)(const char*,int64_t)>();

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

void RegCreateJavaObjFunc(int64_t(*func)(const char*,int64_t))
{
    __android_log_print(ANDROID_LOG_DEBUG, "crosspl", "%s", __PRETTY_FUNCTION__);
    gCreateJavaObjFuncList.push_back(func);
}
void RegDestroyJavaObjFunc(int(*func)(const char*,int64_t))
{
    __android_log_print(ANDROID_LOG_DEBUG, "crosspl", "%s", __PRETTY_FUNCTION__);
    gDestroyJavaObjFuncList.push_back(func);
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

int64_t CrossBase::CreatePlatformObject(const char* cppClassName, int64_t nativeHandle)
{
    __android_log_print(ANDROID_LOG_DEBUG, "crosspl", "%s", __PRETTY_FUNCTION__);

    for(auto func: gCreateJavaObjFuncList) {
        auto javaHandle = func(cppClassName, nativeHandle);
        if(javaHandle != 0) { // success
            return javaHandle;
        }
    }

    // need return before here.
    throw std::runtime_error("CrossPL: Failed to create java object.");
}

void CrossBase::DestroyPlatformObject(const char* cppClassName, int64_t platformHandle)
{
    __android_log_print(ANDROID_LOG_DEBUG, "crosspl", "%s", __PRETTY_FUNCTION__);

    for(auto func: gDestroyJavaObjFuncList) {
        auto javaHandle = func(cppClassName, platformHandle);
        if(javaHandle == 0) { // success
            return;
        }
    }

    // need return before here.
    throw std::runtime_error("CrossPL: Failed to destroy java object.");
}

/***********************************************/
/***** class public function implement  ********/
/***********************************************/
CrossBase::CrossBase(int64_t platformHandle)
    : mPlatformHandle(platformHandle)
{
    if(platformHandle == 0) {
        platformHandle = CreatePlatformObject(typeid(this).name(), reinterpret_cast<int64_t>(this));
    }
}

CrossBase::CrossBase()
    : CrossBase(0)
{
}

CrossBase::~CrossBase()
{
    if(mPlatformHandle == 0) {
        for(auto func: gCreateJavaObjFuncList) {
            int ret = func(typeid(this).name(), mPlatformHandle);
            if(ret == 0) { // success
                break;
            }
        }
    }
}

int64_t CrossBase::getPlatformHandle() const
{
    return mPlatformHandle;
}


/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/
