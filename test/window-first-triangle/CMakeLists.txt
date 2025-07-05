add_executable(vktest-first-triangle main.cpp)
target_include_directories(vktest-first-triangle PUBLIC ./test/window-first-triangle)
target_link_libraries(vktest-first-triangle PUBLIC LIB_STATIC)
