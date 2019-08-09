file( GLOB ContactTest-SOURCES "ContactTest*.cpp")

add_executable(ContactTest)
target_sources(ContactTest PRIVATE
    ${ContactTest-SOURCES})
target_include_directories(ContactTest PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../common"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../platform"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src"
    "${CMAKE_CURRENT_SOURCE_DIR}/../wrapper/json"
    "${CMAKE_INSTALL_PREFIX}/include")
target_link_libraries(ContactTest PUBLIC
    Elastos.SDK.Contact)
configure_file("ContactTestUI.sh" "${CMAKE_CURRENT_BINARY_DIR}/ContactTestUI.sh" COPYONLY)

add_executable(DidChnClientTest)
target_sources(DidChnClientTest PRIVATE DidChnClientTest.cpp)
target_include_directories(DidChnClientTest PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../common"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../platform"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src"
    "${CMAKE_CURRENT_SOURCE_DIR}/../wrapper/json"
    "${CMAKE_INSTALL_PREFIX}/include")
target_link_libraries(DidChnClientTest PUBLIC
    Elastos.SDK.Contact)
