cmake_minimum_required(VERSION 3.4.1)

include( "${CROSSPL_CMAKEFILE}" )

add_library( crosspl SHARED )
target_sources( crosspl PRIVATE
        ${CROSSPL_PROXY_SOURCES}
        utils/Log.cpp
        utils/JniUtils.cpp
        CrossBase.cpp
        CrossRunnable.cpp
)
target_include_directories( crosspl PRIVATE )

find_library( log-lib log )
target_link_libraries( crosspl log )
