cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_COMPILER "C:/llvm-mingw/bin/g++.exe")
set(CMAKE_C_COMPILER "C:/llvm-mingw/bin/gcc.exe")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CPM_SOURCE_CACHE ../cpmcache/ CACHE PATH "The CPM cache path")
include(./cmake/CPM.cmake)
include(./cmake/installpackage.cmake)

project(cppvulkan)
add_compile_options(-std=c++23 -Wall)

# 生成boundlessvk库文件
aux_source_directory(./src/ LIB_SRC)
add_library(LIB_SHARED SHARED ${LIB_SRC})
add_library(LIB_STATIC STATIC ${LIB_SRC})
target_include_directories(LIB_SHARED PUBLIC ./inc/)
target_include_directories(LIB_STATIC PUBLIC ./inc/)
target_link_libraries(LIB_SHARED glfw)
target_link_libraries(LIB_SHARED ${Vulkan_LIBRARIES})
target_link_libraries(LIB_STATIC glfw)
target_link_libraries(LIB_STATIC ${Vulkan_LIBRARIES})
set_target_properties(LIB_SHARED PROPERTIES OUTPUT_NAME "boundlessvk")
set_target_properties(LIB_STATIC PROPERTIES OUTPUT_NAME "boundlessvk")
set(LIBRARY_OUTPUT_PATH ./lib/)

add_subdirectory(./test)
