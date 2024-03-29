cmake_minimum_required(VERSION 3.0)
project(effect_wavebender VERSION 0.3.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Modules)

include(CPM)
include(blink_plugin)

list(APPEND SRC
    ./src/audio.h
    ./src/audio.cpp
    ./src/audio_data.h
    ./src/audio_data.cpp
    ./src/instance.h
    ./src/instance.cpp
    ./src/parameters.h
    ./src/parameters.cpp
    ./src/parameters/bubble.hpp
    ./src/parameters/mix.hpp
    ./src/parameters/pitch.hpp
    ./src/parameters/smoother.hpp
    ./src/parameters/tilt.hpp
    ./src/parameters/xfade_mode.hpp
    ./src/parameters/xfade_size.hpp
    ./src/plugin.h
    ./src/plugin.cpp
)

list(APPEND RES
	./res/icon.png
)

blink_plugin_add_effect("wavebender" "${SRC}" "${RES}")

target_include_directories(${PROJECT_NAME} PRIVATE
	../..
)
