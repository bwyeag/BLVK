aux_source_directory(${CMAKE_SOURCE_DIR}/src/lib SRCFILES)
add_library(BLVKLib STATIC
    # libs/...
    ${SRCFILES}
)
# 指定库的头文件
target_include_directories(BLVKLib PUBLIC ${CMAKE_SOURCE_DIR}/src/inc)
target_link_libraries(BLVKLib PUBLIC VMA)
target_link_libraries(BLVKLib PUBLIC ${Vulkan_LIBRARIES})
# 创建可执行文件目标
add_executable(BLVKMain main.cpp)
# 链接库到可执行文件
target_link_libraries(BLVKMain PUBLIC BLVKLib)
