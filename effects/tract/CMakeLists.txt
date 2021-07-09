cmake_minimum_required(VERSION 3.0)
project(effect_tract VERSION 0.1.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Modules)

include(CPM)
include(blink_plugin)

list(APPEND SRC
    ./src/audio.h
    ./src/audio.cpp
    ./src/audio_data.h
    ./src/audio_data.cpp
    ./src/parameters.h
    ./src/parameters.cpp
    ./src/plugin.h
    ./src/plugin.cpp
	../../shared/resampler.h
	../../shared/tract.h
	../../shared/tract.cpp
	../../shared/tract_parameters.h
	../../shared/tract_parameters.cpp
)

blink_plugin_add(${PROJECT_NAME} "${SRC}")
blink_plugin_set_effect_target_properties("tract")

target_include_directories(${PROJECT_NAME} PRIVATE
	../..
)