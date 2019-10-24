cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions -Wall")
add_subdirectory(libvst)
add_subdirectory(libvst/INTERNAL_APPLICATION_PRIVATE/GLEXAMPLE)
include_directories(libvst/SDK/WinAPI/include)
add_subdirectory(RotatingSquares)
add_subdirectory(waveform)
