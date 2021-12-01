cmake_minimum_required(VERSION 3.0)
project(sampler_classic VERSION 0.2.3)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Modules)

include(CPM)
include(blink_plugin)

list(APPEND SRC
    ./src/audio.h
    ./src/audio.cpp
    ./src/gui.h
    ./src/gui.cpp
    ./src/instance.h
    ./src/instance.cpp
    ./src/plugin.h
    ./src/plugin.cpp
	../../shared/noise_generator.h
	../../shared/noise_generator.cpp
)

blink_plugin_add_sampler("classic" "${SRC}" "")

target_include_directories(${PROJECT_NAME} PRIVATE
	../..
)
