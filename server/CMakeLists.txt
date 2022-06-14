cmake_minimum_required(VERSION 3.18)

project(jwx VERSION 0.1.0 DESCRIPTION "John's Webserver eXperiment")

add_executable(jwx "src/main.cpp" "src/socket_client.cpp" "src/http_client.cpp" "src/http/http_message.cpp" "src/http/http_request.cpp" "src/utils.cpp")
target_include_directories(jwx PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_target_properties(jwx PROPERTIES CMAKE_CXX_STANDARD 20)
set_target_properties(jwx PROPERTIES CMAKE_CXX_STANDARD_REQUIRED TRUE)