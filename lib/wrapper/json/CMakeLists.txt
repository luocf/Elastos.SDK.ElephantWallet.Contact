file(GLOB JSON_SOURCES "*.hpp")
add_library(json OBJECT ${JSON_SOURCES})
target_include_directories(json PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
