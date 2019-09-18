#import <Foundation/Foundation.h>

#import "CrossBase.hpp"
@class CrossBase;
#import "CrossPLUtils.h"


//
//#include <android/log.h>
//#include <vector>
//
namespace crosspl {
//
///***********************************************/
///***** static variables initialize *************/
///***********************************************/
////const char* CrossPLUtils::JavaClassNameBoolean      = "java/lang/Boolean";
////const char* CrossPLUtils::JavaClassNameInteger      = "java/lang/Integer";
////const char* CrossPLUtils::JavaClassNameLong         = "java/lang/Long";
////const char* CrossPLUtils::JavaClassNameDouble       = "java/lang/Double";
//const char* CrossPLUtils::JavaClassNameString                  = "java/lang/String";
//const char* CrossPLUtils::JavaClassNameByteArray               = "[B";
//const char* CrossPLUtils::JavaClassNameRunnable                = "java/lang/Runnable";
//const char* CrossPLUtils::JavaClassNameStringBuffer            = "java/lang/StringBuffer";
//const char* CrossPLUtils::JavaClassNameByteArrayOutputStream   = "java/io/ByteArrayOutputStream";
//
//std::map<const char*, jclass> CrossPLUtils::sJavaClassCache {};
//JavaVM* CrossPLUtils::sJVM = nullptr;
//
//
///***********************************************/
///***** static function implement ***************/
///***********************************************/
//void CrossPLUtils::SetJavaVM(JavaVM* jvm)
//{
//    sJVM = jvm;
//}
//
//std::unique_ptr<JNIEnv, std::function<void(JNIEnv*)>> CrossPLUtils::SafeGetEnv()
//{
//    std::unique_ptr<JNIEnv, std::function<void(JNIEnv*)>> ret;
//    bool needDetach = false;
//    std::thread::id threadId = std::this_thread::get_id();
//
//    JNIEnv* jenv;
//    jint jret = sJVM->GetEnv((void **) &jenv, JNI_VERSION_1_6);
//    if (jret != JNI_OK) {
//        jret = sJVM->AttachCurrentThread(&jenv, nullptr);
//        if (jret != JNI_OK) {
//            throw std::runtime_error("CrossPL: Failed to get jni env, AttachCurrentThread return error");
//        }
//        needDetach = true;
//    }
//
//    auto deleter = [=](JNIEnv* jenv) -> void {
//        EnsureRunOnThread(threadId);
//        if (needDetach == true) {
//            sJVM->DetachCurrentThread();
//        }
//    };
//
//    ret = std::unique_ptr<JNIEnv, std::function<void(JNIEnv*)>>(jenv, deleter);
//
//    return ret;
//}
//
//jclass CrossPLUtils::FindJavaClass(JNIEnv* jenv, const char* className)
//{
//    // CRASH if not found
//    auto found = sJavaClassCache.find(className);
//    if(found == sJavaClassCache.end()) {
//        jclass clazz = jenv->FindClass(className);
//        sJavaClassCache[className] = (jclass) jenv->NewGlobalRef(clazz);
//        jenv->DeleteLocalRef(clazz);
//    }
//
//
//    return sJavaClassCache.at(className);
//}
//
void CrossPLUtils::EnsureRunOnThread(std::thread::id threadId)
{
    std::thread::id currThreadId = std::this_thread::get_id();
    if(currThreadId != threadId) {
      printf("CrossPL: Running on incorrect thread!!!");
      throw std::runtime_error("CrossPL: Running on incorrect thread");
    }
}

std::shared_ptr<const char> CrossPLUtils::SafeCastString(NSString* ocdata)
{
    std::shared_ptr<const char> ret;
    std::thread::id threadId = std::this_thread::get_id();

    if(ocdata == nullptr) {
        return ret; // nullptr
    }

    auto creater = [=]() -> const char* {
        EnsureRunOnThread(threadId);
      
        const char* ptr = [ocdata UTF8String];
        return ptr;
    };
    auto deleter = [=](const char* ptr) -> void {
        EnsureRunOnThread(threadId);
    };

    ret = std::shared_ptr<const char>(creater(), deleter);

    return ret;
}

//std::shared_ptr<std::function<void()>> CrossPLUtils::SafeCastFunction(JNIEnv* jenv, jobject jdata)
//{
//    std::shared_ptr<std::function<void()>> ret;
//    std::thread::id threadId = std::this_thread::get_id();
//
//    if(jdata == nullptr) {
//        return ret; // nullptr
//    }
//
//    auto creater = [=]() -> std::function<void()>* {
//        EnsureRunOnThread(threadId);
//
//        auto ptr = new std::function<void()>([=]() {
//            jclass jclazz = FindJavaClass(jenv, JavaClassNameRunnable);
//            jmethodID jmethod = jenv->GetMethodID(jclazz, "run", "()V");
//            jenv->CallVoidMethod(jdata, jmethod);
//        });
//        return ptr;
//    };
//    auto deleter = [=](std::function<void()>* ptr) -> void {
//        EnsureRunOnThread(threadId);
//        delete ptr;
//    };
//
//    ret = std::shared_ptr<std::function<void()>>(creater(), deleter);
//
//    return ret;
//}
//
std::shared_ptr<std::span<int8_t>> CrossPLUtils::SafeCastByteArray(NSData* ocdata)
{
    std::shared_ptr<std::span<int8_t>> ret;
    std::thread::id threadId = std::this_thread::get_id();

    if(ocdata == nullptr) {
        return ret; // nullptr
    }

    auto creater = [=]() -> std::span<int8_t>* {
        EnsureRunOnThread(threadId);
        const int8_t* arrayPtr = reinterpret_cast<const int8_t*>([ocdata bytes]);
        NSUInteger arrayLen = [ocdata length];
        auto retPtr = new std::span<int8_t>(const_cast<int8_t*>(arrayPtr), arrayLen);
        return retPtr;
    };
    auto deleter = [=](std::span<int8_t>* ptr) -> void {
        EnsureRunOnThread(threadId);
        delete ptr;
    };

    ret = std::shared_ptr<std::span<int8_t>>(creater(), deleter);

    return ret;
}

std::shared_ptr<std::stringstream> CrossPLUtils::SafeCastStringBuffer(NSString** ocdata)
{
    std::shared_ptr<std::stringstream> ret;
    std::thread::id threadId = std::this_thread::get_id();

    if(ocdata == nullptr) {
        return ret; // nullptr
    }

    auto creater = [=]() -> std::stringstream* {
        EnsureRunOnThread(threadId);

        auto str = SafeCastString(*ocdata);
        auto retPtr = new std::stringstream(str.get());

        return retPtr;
    };
    auto deleter = [=](std::stringstream* ptr) -> void {
        EnsureRunOnThread(threadId);
        delete ptr;
    };

    ret = std::shared_ptr<std::stringstream>(creater(), deleter);

    return ret;
}

std::shared_ptr<std::vector<int8_t>> CrossPLUtils::SafeCastByteBuffer(NSData** ocdata)
{
    std::shared_ptr<std::vector<int8_t>> ret;
    std::thread::id threadId = std::this_thread::get_id();

    if(ocdata == nullptr) {
        return ret; // nullptr
    }

    auto creater = [=]() -> std::vector<int8_t>* {
        EnsureRunOnThread(threadId);

        auto bytes = SafeCastByteArray(*ocdata);
        auto retPtr = new std::vector<int8_t>(bytes->data(), bytes->data() + bytes->size());

        return retPtr;
    };
    auto deleter = [=](std::vector<int8_t>* ptr) -> void {
        EnsureRunOnThread(threadId);
        delete ptr;
    };

    ret = std::shared_ptr<std::vector<int8_t>>(creater(), deleter);

    return ret;
}

//std::shared_ptr<_jstring> CrossPLUtils::SafeCastString(JNIEnv* jenv, const char* data)
//{
//    std::shared_ptr<_jstring> ret;
//    std::thread::id threadId = std::this_thread::get_id();
//
//    if (data == nullptr) {
//        return ret; // nullptr
//    }
//
//    auto creater = [=]() -> jstring {
//        EnsureRunOnThread(threadId);
//        jstring ptr = jenv->NewStringUTF(data);
//        return ptr;
//    };
//
//    auto deleter = [=](jstring ptr) -> void {
//        EnsureRunOnThread(threadId);
//        jenv->DeleteLocalRef(ptr);
//    };
//
//    ret = std::shared_ptr<_jstring>(creater(), deleter);
//
//    return ret;
//}
//
//std::shared_ptr<_jbyteArray> CrossPLUtils::SafeCastByteArray(JNIEnv* jenv, const std::span<int8_t>* data)
//{
//    std::shared_ptr<_jbyteArray> ret;
//    std::thread::id threadId = std::this_thread::get_id();
//
//    if(data == nullptr) {
//        return ret; // nullptr
//    }
//
//    auto creater = [=]() -> jbyteArray {
//        EnsureRunOnThread(threadId);
//        jbyteArray jdata = jenv->NewByteArray(data->size());
//        jenv->SetByteArrayRegion(jdata, 0, data->size(), data->data());
//        return jdata;
//    };
//    auto deleter = [=](jbyteArray ptr) -> void {
//        EnsureRunOnThread(threadId);
//        jenv->DeleteLocalRef(ptr);
//    };
//
//    ret = std::shared_ptr<_jbyteArray>(creater(), deleter);
//
//    return ret;
//}
//
//std::shared_ptr<_jobject> CrossPLUtils::SafeCastFunction(JNIEnv* jenv, const std::function<void()>* data)
//{
//    std::shared_ptr<_jobject> ret;
//    std::thread::id threadId = std::this_thread::get_id();
//
//    if(data == nullptr) {
//        return ret; // nullptr
//    }
//
//    auto creater = [&]() -> jobject {
//        EnsureRunOnThread(threadId);
//
//        jclass jclazz = FindJavaClass(jenv, JavaClassNameRunnable);
//
////        jmethodID jconstructor  = jenv->GetMethodID(jclazz, "<init>", "()V");
////        jobject jobj = jenv->NewObject(jclazz, jconstructor);
//
// //       return jobj;
//    };
//    auto deleter = [=](jobject ptr) -> void {
//        EnsureRunOnThread(threadId);
////        jenv->DeleteLocalRef(ptr);
//    };
//
//    ret = std::shared_ptr<_jobject>(creater(), deleter);
//
//    return ret;
//}
//
//std::shared_ptr<_jobject> CrossPLUtils::SafeCastStringBuffer(JNIEnv* jenv, const std::stringstream* data)
//{
//    std::shared_ptr<_jobject> ret;
//    std::thread::id threadId = std::this_thread::get_id();
//
//    if(data == nullptr) {
//        return ret; // nullptr
//    }
//
//    auto creater = [=]() -> jobject {
//        EnsureRunOnThread(threadId);
//
//        jclass jclazz = FindJavaClass(jenv, JavaClassNameStringBuffer);
//
//        jmethodID jconstructor  = jenv->GetMethodID(jclazz, "<init>", "()V");
//        jobject jobj = jenv->NewObject(jclazz, jconstructor);
//
//        SafeCopyStringBufferToJava(jenv, jobj, data);
//
//        return jobj;
//    };
//    auto deleter = [=](jobject ptr) -> void {
//        EnsureRunOnThread(threadId);
//        jenv->DeleteLocalRef(ptr);
//    };
//
//    ret = std::shared_ptr<_jobject>(creater(), deleter);
//
//    return ret;
//}
//
//std::shared_ptr<_jobject> CrossPLUtils::SafeCastByteBuffer(JNIEnv* jenv, const std::vector<int8_t>* data)
//{
//    std::shared_ptr<_jobject> ret;
//    std::thread::id threadId = std::this_thread::get_id();
//
//    if(data == nullptr) {
//        return ret; // nullptr
//    }
//
//    auto creater = [=]() -> jobject {
//        EnsureRunOnThread(threadId);
//
//        jclass jclazz = FindJavaClass(jenv, JavaClassNameByteArrayOutputStream);
//
//        jmethodID jconstructor  = jenv->GetMethodID(jclazz, "<init>", "()V");
//        jobject jobj = jenv->NewObject(jclazz, jconstructor);
//
//        SafeCopyByteBufferToJava(jenv, jobj, data);
//
//        return jobj;
//    };
//    auto deleter = [=](jobject ptr) -> void {
//        EnsureRunOnThread(threadId);
//        jenv->DeleteLocalRef(ptr);
//    };
//
//    ret = std::shared_ptr<_jobject>(creater(), deleter);
//
//    return ret;
//}
//
//int CrossPLUtils::SafeCopyStringBufferToCpp(JNIEnv* jenv, std::stringstream* copyTo, jobject jdata)
//{
//    if(copyTo == nullptr) {
//        return 0; // nullptr
//    }
//
//    auto tmpPtr = SafeCastStringBuffer(jenv, jdata);
//    copyTo->str(""); // clear stringstream
//    copyTo->swap(*tmpPtr);
//
//    return 0;
//}
//
//int CrossPLUtils::SafeCopyByteBufferToCpp(JNIEnv* jenv, std::vector<int8_t>* copyTo, jobject jdata)
//{
//    if(copyTo == nullptr) {
//        return 0; // nullptr
//    }
//
//    auto tmpPtr = SafeCastByteBuffer(jenv, jdata);
//    copyTo->swap(*tmpPtr);
//
//    return 0;
//}

int CrossPLUtils::SafeCopyStringBufferToSwift(NSString** occopyTo, const std::stringstream* data)
{
    if(occopyTo == nullptr) {
        return 0; // nullptr
    }

    *occopyTo = [NSString stringWithUTF8String: data->str().c_str()];

    return 0;
}

int CrossPLUtils::SafeCopyByteBufferToSwift(NSData** occopyTo, const std::vector<int8_t>* data)
{
    if(occopyTo == nullptr) {
        return 0; // nullptr
    }

    *occopyTo = [NSData dataWithBytes: data->data() length: data->size()];

    return 0;
}
//
//void* CrossPLUtils::SafeCastCrossObject(JNIEnv* jenv, jobject jdata)
//{
//    jclass jclazz = jenv->GetObjectClass(jdata);
//
//    jfieldID jfield = jenv->GetFieldID(jclazz, "nativeHandle", "J");
//
//    jlong jnativeHandle = jenv->GetLongField(jdata, jfield);
//
//    return reinterpret_cast<void*>(jnativeHandle);
//}
//
//
///***********************************************/
///***** class public function implement  ********/
///***********************************************/
//
//
///***********************************************/
///***** class protected function implement  *****/
///***********************************************/
//
//
///***********************************************/
///***** class private function implement  *******/
///***********************************************/
//
//
} // namespace crosspl
