cmake_minimum_required(VERSION 3.0)
project(synth_berk VERSION 0.1.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Modules)

include(CPM)
include(blink_plugin)

CPMAddPackage("gh:colugomusic/bleat#master")
cpm_export_variables(bleat)

list(APPEND SRC
    ./src/audio.h
    ./src/audio.cpp
    ./src/audio_data.h
    ./src/audio_data.cpp
    ./src/glottis.h
    ./src/glottis.cpp
    ./src/instance.h
    ./src/instance.cpp
    ./src/parameters.h
    ./src/parameters.cpp
    ./src/plugin.h
    ./src/plugin.cpp
    ./src/simplexnoise1234.h
    ./src/simplexnoise1234.cpp
	../../shared/resampler.h
	../../shared/tract.h
	../../shared/tract.cpp
	../../shared/tract_parameters.h
	../../shared/tract_parameters.cpp
)

blink_plugin_add(${PROJECT_NAME} "${SRC}")
blink_plugin_set_synth_target_properties("berk")

target_include_directories(${PROJECT_NAME} PRIVATE
	../..
)
