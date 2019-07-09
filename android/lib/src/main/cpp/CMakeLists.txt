cmake_minimum_required(VERSION 3.4.1)

#set(CMAKE_C_STANDARD 11)
#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_C_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(PkgConfig REQUIRED)
set(ENV{PKG_CONFIG_PATH}  "${CFG_DEPENDS_DIR}/${ANDROID_ABI}/lib/pkgconfig")
include_directories("${CFG_DEPENDS_DIR}/${ANDROID_ABI}/include")

include( "${CROSSPL_CMAKEFILE}" )

add_subdirectory(base/common)
add_subdirectory(base/src)
add_subdirectory(base/platform)
add_subdirectory(base/wrapper/httpclient)
add_subdirectory(base/wrapper/json)


file( GLOB Elastos.SDK.Contact.Jni-SOURCES "*.cpp")
add_library(Elastos.SDK.Contact.Jni SHARED
            ${CROSSPL_PROXY_SOURCES}
            ${Elastos.SDK.Contact.Jni-SOURCES} )

target_include_directories( Elastos.SDK.Contact.Jni PRIVATE "${CROSSPL_LIB_HEADERDIR}" )
target_link_libraries( Elastos.SDK.Contact.Jni Elastos.SDK.Contact )
target_link_libraries( Elastos.SDK.Contact.Jni ${CROSSPL_LIB_LIBS} )

target_link_libraries( Elastos.SDK.Contact.Jni log )
