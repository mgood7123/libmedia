cmake_minimum_required(VERSION 3.4.1)

add_subdirectory (AndroidBitmap)
add_subdirectory (ReSampler-development)
#add_subdirectory (../../../../../../../AndroidStudioProjects/CCodeTesting/app/src/main/cpp/LIB_GD)
#add_subdirectory (libwaveform)
include_directories (native)
include_directories (libMonitor)
include_directories (ReSampler-development)

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
        aaudio
        ReSampler
        AndroidBitmap
        log
        android
        jnigraphics
        m
)