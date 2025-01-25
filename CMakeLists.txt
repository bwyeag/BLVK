cmake_minimum_required(VERSION 3.10)
project(BLVK VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(subprojects)
include(./cmake/CPM.cmake)
include(./cmake/installpackage.cmake)

add_subdirectory(src)