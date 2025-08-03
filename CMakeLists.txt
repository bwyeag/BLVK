cmake_minimum_required(VERSION 3.20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CPM_SOURCE_CACHE ../cpmcache/ CACHE PATH "The CPM cache path")
include(./cmake/CPM.cmake)
include(./cmake/installpackage.cmake)

project(cppvulkan)
add_compile_options(-std=c++23 -Wall -DDEBUG -g)

# 生成boundlessvk库文件
aux_source_directory(./src/ LIB_SRC)
aux_source_directory(./src/utils/ LIB_SRC_utils)
#aux_source_directory(./src/utils/ LIB_SRC_loader)
#aux_source_directory(./src/utils/ LIB_SRC_platform)
list(APPEND LIB_SRC ${LIB_SRC_utils})

add_library(LIB_STATIC STATIC ${LIB_SRC})
target_include_directories(LIB_STATIC PUBLIC ./inc/)
target_link_libraries(LIB_STATIC glfw)
target_link_libraries(LIB_STATIC Vulkan::Vulkan)
target_include_directories(LIB_STATIC PUBLIC $ENV{VULKAN_SDK}/Include)
set_target_properties(LIB_STATIC PROPERTIES OUTPUT_NAME "boundlessvk")
set(LIBRARY_OUTPUT_PATH ./lib/)

add_subdirectory(./test)
