cmake_minimum_required(VERSION 3.0)
project(sampler_fudge VERSION 0.3.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Modules)

include(CPM)
include(blink_plugin)

list(APPEND SRC
    ./src/audio.h
    ./src/audio.cpp
    ./src/audio_data.h
    ./src/audio_data.cpp
    ./src/controller.h
    ./src/controller.cpp
    ./src/convert.h
    ./src/grain.h
    ./src/gui.h
    ./src/gui.cpp
    ./src/instance.h
    ./src/instance.cpp
    ./src/limits.h
    ./src/parameters.h
    ./src/parameters.cpp
    ./src/parameters/amp.hpp
    ./src/parameters/grain_size.hpp
    ./src/parameters/grain_transpose.hpp
    ./src/parameters/grain_uniformity.hpp
    ./src/parameters/harmonics_amount.hpp
    ./src/parameters/harmonics_scale.hpp
    ./src/parameters/harmonics_spread.hpp
    ./src/parameters/loop.hpp
    ./src/parameters/noise_amount.hpp
    ./src/parameters/noise_color.hpp
    ./src/parameters/noise_mode.hpp
    ./src/parameters/noise_width.hpp
    ./src/parameters/pan.hpp
    ./src/parameters/pitch.hpp
    ./src/parameters/reverse.hpp
    ./src/parameters/sample_offset.hpp
    ./src/parameters/speed.hpp
    ./src/particle.h
    ./src/particle.cpp
    ./src/plugin.h
    ./src/plugin.cpp
	./src/sample_analysis.h
	./src/sample_analysis.cpp
	../shared/autocorrelation.hpp
	../../shared/noise_generator.h
	../../shared/noise_generator.cpp
)

blink_plugin_add_sampler("fudge" "${SRC}" "")

target_include_directories(${PROJECT_NAME} PRIVATE
	..
	../..
)
