add_library(utfcpp INTERFACE)
target_include_directories(utfcpp INTERFACE ./)
target_compile_features(utfcpp INTERFACE cxx_std_20)