cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions -Wall")

add_library(VSTRotatingSquaresWindows SHARED gles3jni.cpp RendererES2.cpp RendererES3.cpp mainWindows.cpp)
target_link_libraries(VSTRotatingSquaresWindows WINAPI GLESv3 android EGL log m)

add_library(VSTRotatingSquaresLinux SHARED gles3jni.cpp RendererES2.cpp RendererES3.cpp mainLinux.cpp)
target_link_libraries(VSTRotatingSquaresLinux GLESv3 android EGL log m)