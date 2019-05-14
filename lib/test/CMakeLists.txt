add_executable(ContactTest)
target_sources(ContactTest PRIVATE ContactTest.cpp)
target_include_directories(ContactTest PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    "${CMAKE_INSTALL_PREFIX}/include")
target_link_libraries(ContactTest PUBLIC
    Elastos.SDK.Contact)

add_executable(BlkChnClientTest)
target_sources(BlkChnClientTest PRIVATE BlkChnClientTest.cpp)
target_include_directories(BlkChnClientTest PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../common"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src"
    "${CMAKE_CURRENT_SOURCE_DIR}/../wrapper/json"
    "${CMAKE_INSTALL_PREFIX}/include")
target_link_libraries(BlkChnClientTest PUBLIC
    Elastos.SDK.Contact)
