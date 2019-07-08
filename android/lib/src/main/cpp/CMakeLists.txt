cmake_minimum_required(VERSION 3.4.1)

find_package(PkgConfig REQUIRED)
set(ENV{PKG_CONFIG_PATH}  "${CFG_DEPENS_DIR}/${ANDROID_ABI}/lib/pkgconfig")

#add_subdirectory(../../../../../lib/common ../../../../../build/android)
#add_subdirectory(../../../../../lib/src  ../../../../../build/android)
#add_subdirectory(../../../../../lib/platform ../../../../../build/android)
#add_subdirectory(../../../../../lib/wrapper/httpclient ../../../../../build/android)
#add_subdirectory(../../../../../lib/wrapper/json ../../../../../build/android)
add_subdirectory(base/common)

add_library( Elastos.SDK.Contact SHARED
        BlockChainNode_jni.cpp )

file(GLOB Elastos.SDK.Contact.C-AllLibs "${CFG_ELASTOS_SDK_WALLET_C_DIR}/jni/${ANDROID_ABI}/*")
file(GLOB Elastos.SDK.Contact.C-Libraries ${Elastos.SDK.Contact.C-AllLibs} ${Elastos.SDK.Contact.C-AllLibs}) ## fix library order issue
file(GLOB Elastos.SDK.Contact.C-HeaderDir "${CFG_ELASTOS_SDK_WALLET_C_DIR}/jni/include")
target_include_directories( Elastos.SDK.Contact PRIVATE "${Elastos.SDK.Contact.C-HeaderDir}" )
target_link_libraries( Elastos.SDK.Contact "${Elastos.SDK.Contact.C-Libraries}" )

target_link_libraries( Elastos.SDK.Contact log )
