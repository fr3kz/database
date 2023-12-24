cmake_minimum_required(VERSION 3.26)
project(kacperekprojekt)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(fmt)

add_executable(kacperekprojekt main.cpp)

target_link_libraries(kacperekprojekt fmt::fmt)
