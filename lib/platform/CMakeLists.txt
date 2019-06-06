file( GLOB PLATFORM_SOURCES "*.cpp" )

add_library(platform OBJECT ${PLATFORM_SOURCES})
target_include_directories(platform PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include")
target_link_libraries(platform common)
