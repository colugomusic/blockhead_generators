cmake_minimum_required(VERSION 3.0)
project(synth_test VERSION 0.4.1)

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
    ./src/instance.h
    ./src/instance.cpp
    ./src/parameters.h
    ./src/parameters.cpp
    ./src/parameters/amp.hpp
    ./src/parameters/carrier_fm.hpp
    ./src/parameters/carrier_pitch.hpp
    ./src/parameters/carrier_waveform.hpp
    ./src/parameters/modulator_fm.hpp
    ./src/parameters/modulator_pitch.hpp
    ./src/parameters/pan.hpp
    ./src/plugin.h
    ./src/plugin.cpp
	../../shared/noise_generator.h
	../../shared/noise_generator.cpp
)

list(APPEND RES
	./res/icon.png
)

blink_plugin_add_synth("test" "${SRC}" "${RES}")

target_include_directories(${PROJECT_NAME} PRIVATE
	${bleat_SOURCE_DIR}/include
	../..
)
