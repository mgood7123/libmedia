cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wno-unused-function")

add_subdirectory (SDK/oboe)
include_directories (SDK/oboe)
include_directories (native)

add_library (
        PlayerOboe SHARED
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
        log
        android
        oboe
        jnigraphics
        m
)