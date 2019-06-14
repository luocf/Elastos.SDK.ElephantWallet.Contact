pkg_search_module(pkg-openssl REQUIRED openssl)
message(STATUS "openssl include path: ${pkg-openssl_INCLUDE_DIRS}")
message(STATUS "openssl library path: ${pkg-openssl_STATIC_LDFLAGS}")

file( GLOB COMMON_SOURCES "*.cpp" )

add_library(common OBJECT ${COMMON_SOURCES})
target_include_directories(common PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${pkg-openssl_INCLUDE_DIRS}")

target_link_libraries(common PUBLIC
    ${pkg-openssl_STATIC_LDFLAGS})
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Android")
    target_link_libraries(common PUBLIC
        log)
endif()
