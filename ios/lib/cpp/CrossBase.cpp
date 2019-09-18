//
//  CrossBase.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include "CrossBase.hpp"

//#include <android/log.h>
//#include <CrossPLFactory.hpp>
#include <list>
#include <string>

/***********************************************/
/***** static variables initialize *************/
/***********************************************/


/***********************************************/
/***** static function implement ***************/
/***********************************************/
static auto gCreateCppObjFuncList = std::list<int64_t(*)(const char*)>();
static auto gDestroyCppObjFuncList = std::list<int(*)(const char*,int64_t)>();
static auto gCreateswiftObjFuncList = std::list<int64_t(*)(const char*,int64_t)>();
static auto gDestroyswiftObjFuncList = std::list<int(*)(const char*,int64_t)>();

void RegCreateCppObjFunc(int64_t(*func)(const char*))
{
//    __android_log_print(ANDROID_LOG_DEBUG, "crosspl", "%s", __PRETTY_FUNCTION__);
    gCreateCppObjFuncList.push_back(func);
}
void RegDestroyCppObjFunc(int(*func)(const char*,int64_t))
{
//    __android_log_print(ANDROID_LOG_DEBUG, "crosspl", "%s", __PRETTY_FUNCTION__);
    gDestroyCppObjFuncList.push_back(func);
}

void RegCreateswiftObjFunc(int64_t(*func)(const char*,int64_t))
{
//    __android_log_print(ANDROID_LOG_DEBUG, "crosspl", "%s", __PRETTY_FUNCTION__);
    gCreateswiftObjFuncList.push_back(func);
}
void RegDestroyswiftObjFunc(int(*func)(const char*,int64_t))
{
//    __android_log_print(ANDROID_LOG_DEBUG, "crosspl", "%s", __PRETTY_FUNCTION__);
    gDestroyswiftObjFuncList.push_back(func);
}

namespace crosspl {
namespace native {

int64_t CrossBase::CreateNativeObject(const char* swiftClassName)
{
//    __android_log_print(ANDROID_LOG_DEBUG, "crosspl", "%s", __PRETTY_FUNCTION__);

    for(auto func: gCreateCppObjFuncList) {
        auto cppHandle = func(swiftClassName);
        if(cppHandle != 0) { // success
            return cppHandle;
        }
    }

    // need return before here.
    auto ex = std::string("CrossPL: Failed to create cpp object.") + swiftClassName;
    throw std::runtime_error(ex);
}

void CrossBase::DestroyNativeObject(const char* swiftClassName, int64_t nativeHandle)
{
//    __android_log_print(ANDROID_LOG_DEBUG, "crosspl", "%s", __PRETTY_FUNCTION__);

    for(auto func: gDestroyCppObjFuncList) {
        auto cppHandle = func(swiftClassName, nativeHandle);
        if(cppHandle == 0) { // success
            return;
        }
    }

    // need return before here.
    auto ex = std::string("CrossPL: Failed to destroy cpp object.") + swiftClassName;
    throw std::runtime_error(ex);
}

int64_t CrossBase::CreatePlatformObject(const char* cppClassName, int64_t nativeHandle)
{
//    __android_log_print(ANDROID_LOG_DEBUG, "crosspl", "%s", __PRETTY_FUNCTION__);

    for(auto func: gCreateswiftObjFuncList) {
        auto swiftHandle = func(cppClassName, nativeHandle);
        if(swiftHandle != 0) { // success
            return swiftHandle;
        }
    }

    // need return before here.
    auto ex = std::string("CrossPL: Failed to create swift object.") + cppClassName;
    throw std::runtime_error(ex);
}

void CrossBase::DestroyPlatformObject(const char* cppClassName, int64_t platformHandle)
{
//    __android_log_print(ANDROID_LOG_DEBUG, "crosspl", "%s", __PRETTY_FUNCTION__);

    for(auto func: gDestroyswiftObjFuncList) {
        auto swiftHandle = func(cppClassName, platformHandle);
        if(swiftHandle == 0) { // success
            return;
        }
    }

    // need return before here.
    auto ex = std::string("CrossPL: Failed to destroy swift object.") + cppClassName;
    throw std::runtime_error(ex);
}

/***********************************************/
/***** class public function implement  ********/
/***********************************************/
//CrossBase::CrossBase(int64_t platformHandle)
//    : mPlatformHandle(platformHandle)
//{
//    if(platformHandle == 0) {
//        platformHandle = CreatePlatformObject(typeid(this).name(), reinterpret_cast<int64_t>(this));
//    }
//}

CrossBase::CrossBase()
//    : CrossBase(0)
    : mPlatformHandle(0)
{
}

CrossBase::~CrossBase()
{
//    if(mPlatformHandle == 0) {
//        for(auto func: gDestroyswiftObjFuncList) {
//            int ret = func(typeid(this).name(), mPlatformHandle);
//            if(ret == 0) { // success
//                break;
//            }
//        }
//    }
}

void CrossBase::bindPlatformHandle(int64_t platformHandle)
{
    mPlatformHandle = platformHandle;
}

void CrossBase::unbindPlatformHandle(int64_t platformHandle)
{
    mPlatformHandle = 0;
}

int64_t CrossBase::getPlatformHandle() const
{
    return mPlatformHandle;
}

  __attribute__((constructor)) static void myinit() {
    printf("=============== I'm initializing..\n");
  }

/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/

} // namespace native
} // namespace crosspl
