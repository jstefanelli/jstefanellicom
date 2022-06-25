cmake_minimum_required(VERSION 3.18)

project(jwx VERSION 0.1.0 DESCRIPTION "John's Webserver eXperiment")

option(JWX_STATIC_LIBSTDCXX "Statically link libstdc++ (use with GCC only!)" OFF)

add_executable(jwx "src/main.cpp" "src/socket_client.cpp" "src/http_client.cpp" "src/utils.cpp"
	"src/http/http_message.cpp" "src/http/http_request.cpp" "src/http/http_response.cpp"
	"src/cache/cached_file.cpp" "src/cache/cache_mgr.cpp")
target_include_directories(jwx PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_definitions(jwx PRIVATE JWX_VERSION="0.1.0")
if (UNIX AND NOT APPLE)
	target_link_libraries(jwx pthread)
endif()

if(JWX_STATIC_LIBSTDCXX)
	target_link_options(jwx PRIVATE -static-libstdc++ -static-libgcc)
endif()

set_target_properties(jwx PROPERTIES CXX_STANDARD 20)
set_target_properties(jwx PROPERTIES CXX_STANDARD_REQUIRED TRUE)