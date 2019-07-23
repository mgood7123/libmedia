cmake_minimum_required(VERSION 3.4.1)

add_subdirectory(Kernel)

add_library(WINAPI SHARED
        WindowsAPIMessage.cpp
        WindowsAPIHander.cpp
        WindowsAPIProcess.cpp
        WindowsAPIThread.cpp
        Locker.cpp
        )
target_link_libraries(WINAPI WinKernel log)