cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions -Wall")
add_library(RotatingSquares SHARED
            gles3jni.cpp
            RendererES2.cpp
            RendererES3.cpp)

target_link_libraries(RotatingSquares
        GLESv3
        android
        EGL
        log
        m)