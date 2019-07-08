#include <jni.h>
#include <string>
//#include "BlockChainNode.h"

//using namespace elastos;

//JNICALL jlong native_newBlockNode(JNIEnv* env, jobject jobj, jstring url) {
//    const char* urlStr = env->GetStringUTFChars(url, nullptr);
//    std::shared_ptr<BlockChainNode>* node = new std::shared_ptr<BlockChainNode>();
//    *node = std::make_shared<BlockChainNode>(urlStr);
//    env->ReleaseStringUTFChars(url, urlStr);
//
//    return (jlong)node;
//
//}
//
//JNICALL void native_setUrl(JNIEnv* env, jobject jobj, jlong obj, jstring url) {
//    if (!obj) return;
//
//    BlockChainNode* node = (BlockChainNode*)obj;
//    const char* urlStr = env->GetStringUTFChars(url, nullptr);
//    node->SetUrl(urlStr);
//    env->ReleaseStringUTFChars(url, urlStr);
//}
//
//JNICALL jstring native_getUrl(JNIEnv* env, jobject jobj, jlong obj) {
//    if (!obj) return env->NewStringUTF("");
//
//    std::shared_ptr<BlockChainNode>* node = (std::shared_ptr<BlockChainNode>*)obj;
//
//    std::string url = (*node)->GetUrl();
//    return env->NewStringUTF(url.c_str());
//}
//
//JNICALL void native_setCertificate(JNIEnv* env, jobject jobj, jlong obj, jstring cert) {
//    if (!obj) return;
//
//    std::shared_ptr<BlockChainNode>* node = (std::shared_ptr<BlockChainNode>*)obj;
//
//    const char* certStr = env->GetStringUTFChars(cert, nullptr);
//    (*node)->SetCertificate(certStr);
//    env->ReleaseStringUTFChars(cert, certStr);
//}
//
//JNICALL void native_destroyNode(JNIEnv* env, jobject jobj, jlong obj) {
//    if (!obj) return;
//    std::shared_ptr<BlockChainNode>* node = (std::shared_ptr<BlockChainNode>*)obj;
//    node->reset();
//    delete node;
//}
//
//static const JNINativeMethod gMethods[] = {
//        {"native_newBlockNode", "(Ljava/lang/String;)J", (void*)native_newBlockNode},
//        {"native_setUrl", "(JLjava/lang/String;)V", (void*)native_setUrl},
//        {"native_getUrl", "(J)Ljava/lang/String;", (void*)native_getUrl},
//        {"native_setCertificate", "(JLjava/lang/String;)V", (void*)native_setCertificate},
//        {"native_destroyNode", "(J)V", (void*)native_destroyNode},
//};
//
//int registerBlockChainNodeMethod(JNIEnv * env) {
//    jclass clazz = env->FindClass("org/elastos/sdk/wallet/BlockChainNode");
//    if (clazz == NULL) return JNI_FALSE;
//
//    int ret = env->RegisterNatives(clazz, gMethods, sizeof(gMethods)/sizeof(JNINativeMethod));
//    if (ret < 0) return ret;
//
//    return JNI_TRUE;
//};
