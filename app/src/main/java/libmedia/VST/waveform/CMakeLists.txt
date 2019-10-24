cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions -Wall")

add_subdirectory(SDL2-2.0.9)

add_library(VSTWaveform SHARED SDL.cpp)

target_link_libraries(VSTWaveform
        SDL2
        android
        EGL
        log
        m)