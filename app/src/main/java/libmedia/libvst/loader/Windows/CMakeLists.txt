cmake_minimum_required(VERSION 3.4.1)

add_subdirectory(kernel)

add_library(Windows_API SHARED
        WindowsAPIMessage.cpp
        WindowsAPIHander.cpp
        WindowsAPIProcess.cpp
        WindowsAPIThread.cpp
        )
target_link_libraries(Windows_API Windows_API_KERNEL log)