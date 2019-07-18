cmake_minimum_required(VERSION 3.4.1)

add_library(Windows_API_KERNEL SHARED
        WindowsAPITable.cpp
        WindowsAPIObject.cpp
        WindowsAPIHandle.cpp
        )
target_link_libraries(Windows_API_KERNEL)