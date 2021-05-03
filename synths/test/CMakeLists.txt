cmake_minimum_required(VERSION 3.0)
project(synth_test VERSION 0.1.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Modules)

include(CPM)
include(blink_plugin)

CPMAddPackage("gh:colugomusic/bleat#master")
cpm_export_variables(bleat)

list(APPEND SRC
    ./src/audio.h
    ./src/audio.cpp
    ./src/plugin.h
    ./src/plugin.cpp
	../../shared/noise_generator.h
	../../shared/noise_generator.cpp
)

add_library(${PROJECT_NAME} SHARED ${SRC})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${blink_SOURCE_DIR}/include
    ${blink_SOURCE_DIR}/plugin
    ${bleat_SOURCE_DIR}/include
	${madronalib_SOURCE_DIR}/source
    ../..
)

blink_plugin_set_synth_target_properties("test")