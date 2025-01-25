add_library(VMA INTERFACE)
target_include_directories(VMA INTERFACE ./)
target_compile_features(VMA INTERFACE cxx_std_17)