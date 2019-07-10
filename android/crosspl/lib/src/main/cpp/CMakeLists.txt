cmake_minimum_required(VERSION 3.4.1)

include( "${CROSSPL_DIR}/crosspl.cmake" )

file( GLOB crosspl-SOURCES "*.cpp" "utils/*.cpp")
add_library( crosspl SHARED
        ${crosspl-SOURCES}
        ${CROSSPL_PROXY_SOURCES})

target_link_libraries( crosspl log )
