cmake_minimum_required(VERSION 3.4.1)

add_subdirectory (SDK/oboe)
add_subdirectory (AndroidBitmap)
add_subdirectory (resampler/libsndfile)
add_subdirectory (resampler/fftw-3.3.8)
add_subdirectory (resampler/ReSampler)

include_directories (SDK/oboe)
include_directories (native)
include_directories (libMonitor)

add_library (
        PlayerOboe SHARED
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
        sndfile
        fftw3
        ReSampler
        AndroidBitmap
        log
        android
        oboe
        jnigraphics
        m
)