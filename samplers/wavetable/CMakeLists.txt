cmake_minimum_required(VERSION 3.0)
project(sampler_wavetable)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Modules)

include(CPM)
include(blink_lib)

list(APPEND SRC
    ./src/audio.h
    ./src/audio.cpp
    ./src/audio_data.h
    ./src/gui.h
    ./src/gui.cpp
    ./src/parameters.h
    ./src/parameters.cpp
    ./src/plugin.h
    ./src/plugin.cpp
	./src/sample_analysis.h
	./src/sample_analysis.cpp
	../shared/autocorrelation.hpp
)

add_library(${PROJECT_NAME} SHARED ${SRC})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${blink_SOURCE_DIR}/include
    ${blink_SOURCE_DIR}/plugin
	${madronalib_SOURCE_DIR}/source
    ../shared
)

blink_plugin_set_sampler_target_properties("wavetable")