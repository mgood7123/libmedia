cmake_minimum_required(VERSION 3.4.1)

add_subdirectory (SDK/oboe)
add_subdirectory (AndroidBitmap)
#set(BUILD_SHARED_LIBS_SAVED "${BUILD_SHARED_LIBS}")
#set(BUILD_PROGRAMS_SAVED "${BUILD_PROGRAMS}")
#set(BUILD_EXAMPLES_SAVED "${BUILD_EXAMPLES}")
#set(BUILD_TESTING_SAVED "${BUILD_TESTING}")
#set(BUILD_SHARED_LIBS ON)
#set(BUILD_PROGRAMS OFF)
#set(BUILD_EXAMPLES OFF)
#set(BUILD_TESTING OFF)
add_subdirectory (resampler/libsndfile)
#set(BUILD_SHARED_LIBS "${BUILD_SHARED_LIBS_SAVED}")
#set(BUILD_PROGRAMS "${BUILD_PROGRAMS_SAVED}")
#set(BUILD_EXAMPLES "${BUILD_EXAMPLES_SAVED}")
#set(BUILD_TESTING "${BUILD_TESTING_SAVED}")
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