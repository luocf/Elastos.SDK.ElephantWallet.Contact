pkg_search_module(pkg-Elastos.SDK.Keypair.C REQUIRED Elastos.SDK.Keypair.C)
message(STATUS "Elastos.SDK.Keypair.C include path: ${pkg-Elastos.SDK.Keypair.C_INCLUDE_DIRS}")
message(STATUS "Elastos.SDK.Keypair.C library path: ${pkg-Elastos.SDK.Keypair.C_STATIC_LDFLAGS}")

file( GLOB Elastos.SDK.Contact-SOURCES "*.cpp" "*/*.cpp")
file( GLOB Elastos.SDK.Contact-HEADERS "../include/*.hpp" )

add_library(Elastos.SDK.Contact)
target_sources(Elastos.SDK.Contact PRIVATE
	${Elastos.SDK.Contact-SOURCES})
#target_sources(Elastos.SDK.Contact PRIVATE
	#$<TARGET_OBJECTS:thirdparty>)
target_include_directories(Elastos.SDK.Contact PRIVATE
	"${pkg-Elastos.SDK.Keypair.C_INCLUDE_DIRS}"
	"${CMAKE_CURRENT_SOURCE_DIR}/../include")
target_link_libraries(Elastos.SDK.Contact PUBLIC
	${pkg-Elastos.SDK.Keypair.C_STATIC_LDFLAGS})
#target_link_libraries(Elastos.SDK.Contact PRIVATE
    #thirdparty)
if("${CFG_TARGET_PLATFORM}" STREQUAL "Android")
    target_link_libraries(Elastos.SDK.Contact PRIVATE log)
endif()

set_target_properties(Elastos.SDK.Contact PROPERTIES PUBLIC_HEADER "${Elastos.SDK.Contact-HEADERS}")
install(TARGETS Elastos.SDK.Contact
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		PUBLIC_HEADER DESTINATION include/Elastos.SDK.Contact)
