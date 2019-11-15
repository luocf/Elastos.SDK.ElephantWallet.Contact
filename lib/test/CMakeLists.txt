file( GLOB ContactTest-SOURCES "*.cpp")

add_executable(ContactTest)
target_sources(ContactTest PRIVATE
    ${ContactTest-SOURCES})
target_include_directories(ContactTest PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../common"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../platform"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src"
    "${CMAKE_CURRENT_SOURCE_DIR}/../api"
    "${CMAKE_CURRENT_SOURCE_DIR}/../wrapper/json"
    "${CMAKE_CURRENT_SOURCE_DIR}/../crosspl"
    "${CMAKE_INSTALL_PREFIX}/include")
target_link_libraries(ContactTest PUBLIC
    Elastos.SDK.Contact)
