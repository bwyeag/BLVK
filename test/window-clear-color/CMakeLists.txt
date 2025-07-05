add_executable(vktest-clear-color main.cpp)
target_include_directories(vktest-clear-color PUBLIC ./test/window-clear-color)
target_link_libraries(vktest-clear-color PUBLIC LIB_STATIC)
