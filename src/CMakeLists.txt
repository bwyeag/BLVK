set(SRCFILES 
    lib/core/bl_init.cpp 
    lib/bl_output.cpp)
add_library(BLVKLib STATIC
    # libs/...
    ${SRCFILES}
)
# 指定库的头文件
target_include_directories(BLVKLib PUBLIC ${CMAKE_SOURCE_DIR}/src/inc)
target_include_directories(BLVKLib PUBLIC D:/vulkanSDK/Include)
target_link_libraries(BLVKLib PUBLIC glfw)
target_link_libraries(BLVKLib PUBLIC VMA)
target_link_libraries(BLVKLib PUBLIC Vulkan)
# 创建可执行文件目标
add_executable(BLVKMain main.cpp)
# 链接库到可执行文件
target_link_libraries(BLVKMain PUBLIC BLVKLib)
