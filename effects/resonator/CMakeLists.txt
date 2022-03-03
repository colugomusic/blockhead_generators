cmake_minimum_required(VERSION 3.0)
project(effect_resonator VERSION 0.2.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Modules)

include(blink_plugin)

list(APPEND SRC
    ./src/audio.h
    ./src/audio.cpp
    ./src/audio_data.h
    ./src/audio_data.cpp
    ./src/convert.h
    ./src/instance.h
    ./src/instance.cpp
    ./src/parameters.h
    ./src/parameters.cpp
    ./src/parameters/damper.hpp
    ./src/parameters/feedback.hpp
    ./src/parameters/fm_amount.hpp
    ./src/parameters/fm_ratio.hpp
    ./src/parameters/harmonics_amount.hpp
    ./src/parameters/harmonics_scale.hpp
    ./src/parameters/harmonics_scale_snap_amount.hpp
    ./src/parameters/harmonics_spread.hpp
    ./src/parameters/harmonics_width.hpp
    ./src/parameters/mix.hpp
    ./src/parameters/pitch.hpp
    ./src/plugin.h
    ./src/plugin.cpp
)

list(APPEND RES
    ./res/icon.png
)

blink_plugin_add_effect("resonator" "${SRC}" "${RES}")

target_include_directories(effect_resonator PRIVATE
	../..
)
