cmake_minimum_required(VERSION 3.7)

add_definitions(-std=c++17)

set(CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")

project(SimpleChat)

add_executable(SimpleChat-Client src/ClientApp.cpp)
add_executable(SimpleChat-Server src/ServerApp.cpp)
