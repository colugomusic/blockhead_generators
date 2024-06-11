cmake_minimum_required(VERSION 3.0)
project(sampler_fudge VERSION 0.4.2)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Modules)

include(CPM)
include(plugin)

list(APPEND SRC
    ./src/audio_data.h
    ./src/controller.h
    ./src/controller.cpp
    ./src/convert.h
    ./src/draw.hpp
    ./src/dsp.hpp
    ./src/grain.h
    ./src/limits.h
    ./src/model.h
    ./src/particle.h
    ./src/particle.cpp
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
