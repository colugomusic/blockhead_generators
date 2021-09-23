cmake_minimum_required(VERSION 3.0)
project(synth_test VERSION 0.1.2)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Modules)

include(CPM)
include(blink_plugin)

CPMAddPackage("gh:colugomusic/bleat#master")
cpm_export_variables(bleat)

list(APPEND SRC
    ./src/audio.h
    ./src/audio.cpp
    ./src/instance.h
    ./src/instance.cpp
    ./src/plugin.h
    ./src/plugin.cpp
	../../shared/noise_generator.h
	../../shared/noise_generator.cpp
)

blink_plugin_add_synth("test" "${SRC}")

target_include_directories(${PROJECT_NAME} PRIVATE
	${bleat_SOURCE_DIR}/include
	../..
)
