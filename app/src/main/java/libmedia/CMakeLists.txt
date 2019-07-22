cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
add_subdirectory (SDK/oboe)
add_subdirectory (AndroidBitmap)
add_subdirectory (ReSampler)
add_subdirectory (VST)

include_directories (SDK/oboe)
include_directories (native)
include_directories (libMonitor)

add_library (
        PlayerOboe SHARED
        shell/env.cpp
        shell/regex_str.cpp
        libMonitor/MonitorPool.cpp
        native/AudioTime.cpp
        Oboe/Generator.cpp
        Oboe/AudioEngine.cpp
        Oboe/audio/SoundRecording.cpp
        Oboe/audio/Mixer.cpp
        Oboe/Player.cpp
        waveform/timestats.cpp
        waveform/waveform.cpp
        waveform/TimeStretch.cpp
        waveform/AudioTools.cpp
)

target_link_libraries (
        PlayerOboe
        ReSampler
        AndroidBitmap
        log
        android
        oboe
        jnigraphics
        m
)