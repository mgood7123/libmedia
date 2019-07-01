cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wno-unused-function")

add_library (
        AndroidBitmap SHARED
        bitmap.cpp
        pixelTypes.cpp
        ../waveform/timestats.cpp
)

target_link_libraries (
        AndroidBitmap
        log
)