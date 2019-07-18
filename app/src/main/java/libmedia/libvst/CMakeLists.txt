cmake_minimum_required(VERSION 3.4.1)
add_subdirectory(loader/Windows)
add_library(internal_loader SHARED loader/internal_loader.cpp)
target_link_libraries(internal_loader log Windows_API)