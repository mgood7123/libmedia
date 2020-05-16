cmake_minimum_required(VERSION 3.4.1)

add_subdirectory (FFMPEG-ANDROID)
include_directories(${FFMPEG-ANDROID-INCLUDE})

add_subdirectory (AndroidBitmap)
#add_subdirectory (ReSampler-development)
include_directories (native)
include_directories (date/include)
include_directories (libMonitor)
include_directories (ReSampler-development)

add_library (
        AudioEngine SHARED
        shell/env.cpp
        shell/regex_str.cpp
        libMonitor/MonitorPool.cpp
        native/AudioTime.cpp
        AudioEngine/Generator.cpp
        AudioEngine/AudioEngine.cpp
        AudioEngine/audio/SoundRecording.cpp
        AudioEngine/audio/Mixer.cpp
        AudioEngine/Player.cpp
        waveform/timestats.cpp
        waveform/waveform.cpp
        waveform/TimeStretch.cpp
        waveform/AudioTools.cpp
)

target_link_libraries (
        AudioEngine
        aaudio
#        ReSampler
        AndroidBitmap
        log
        android
        jnigraphics
        m
        FFMPEG-ANDROID
)