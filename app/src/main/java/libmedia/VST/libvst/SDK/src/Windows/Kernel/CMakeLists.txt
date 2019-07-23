cmake_minimum_required(VERSION 3.4.1)

add_library(WinKernel SHARED
        WindowsAPIKernel.cpp
        WindowsAPITable.cpp
        WindowsAPIObject.cpp
        WindowsAPIHandle.cpp
        )
target_link_libraries(WinKernel)