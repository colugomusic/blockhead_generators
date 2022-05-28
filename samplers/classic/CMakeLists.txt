cmake_minimum_required(VERSION 3.0)
project(sampler_classic VERSION 0.3.1)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Modules)

include(CPM)
include(blink_plugin)

list(APPEND SRC
    ./src/audio.h
    ./src/audio.cpp
    ./src/audio_data.h
    ./src/audio_data.cpp
    ./src/gui.h
    ./src/gui.cpp
    ./src/instance.h
    ./src/instance.cpp
    ./src/parameters.h
    ./src/parameters.cpp
    ./src/parameters/amp.hpp
    ./src/parameters/loop.hpp
    ./src/parameters/noise_amount.hpp
    ./src/parameters/noise_color.hpp
    ./src/parameters/noise_mode.hpp
    ./src/parameters/noise_width.hpp
    ./src/parameters/pan.hpp
    ./src/parameters/pitch.hpp
    ./src/parameters/reverse.hpp
    ./src/parameters/sample_offset.hpp
    ./src/plugin.h
    ./src/plugin.cpp
	../../shared/noise_generator.h
	../../shared/noise_generator.cpp
)

blink_plugin_add_sampler("classic" "${SRC}" "")

target_include_directories(${PROJECT_NAME} PRIVATE
	../..
)
