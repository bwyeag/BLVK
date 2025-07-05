add_executable(vktest main.cpp)
target_include_directories(vktest PUBLIC ./test)
target_link_libraries(vktest PUBLIC LIB_STATIC)
