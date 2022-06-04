cmake_minimum_required(VERSION 3.20)

project(jwx VERSION 0.1.0 DESCRIPTION "John stefanelli Webserver eXperiment")

add_executable(jwx "src/main.cpp")
set_target_properties(PROPERTIES CMAKE_CXX_STANDARD 20)
set_target_properties(PROPERTIES CMAKE_CXX_STANDARD_REQUIRED TRUE)