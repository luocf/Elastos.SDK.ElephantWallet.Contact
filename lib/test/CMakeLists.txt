file( GLOB Elastos.SDK.Contact.Test-SOURCES "*.cpp" "*/*.cpp")
file( GLOB Elastos.SDK.Contact.Test-HEADERS "../include/*.hpp" )

add_executable(Elastos.SDK.Contact.Test)
target_sources(Elastos.SDK.Contact.Test PRIVATE
	${Elastos.SDK.Contact.Test-SOURCES})
target_include_directories(Elastos.SDK.Contact.Test PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/../include")
target_link_libraries(Elastos.SDK.Contact.Test PUBLIC
    Elastos.SDK.Contact)
