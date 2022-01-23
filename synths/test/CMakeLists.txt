cmake_minimum_required(VERSION 3.0)
project(synth_test VERSION 0.3.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Modules)

include(CPM)
include(blink_plugin)

CPMAddPackage("gh:colugomusic/bleat#master")
cpm_export_variables(bleat)

list(APPEND SRC_HEADERS
    ./src/audio.h
    ./src/audio_data.h
    ./src/instance.h
    ./src/parameters.h
    ./src/plugin.h
)

list(APPEND SRC_PARAMETERS
    ./src/parameters/amp.hpp
    ./src/parameters/carrier_fm.hpp
    ./src/parameters/carrier_pitch.hpp
    ./src/parameters/carrier_waveform.hpp
    ./src/parameters/modulator_fm.hpp
    ./src/parameters/modulator_pitch.hpp
)

list(APPEND SRC_CPP
    ./src/audio.cpp
    ./src/audio_data.cpp
    ./src/instance.cpp
    ./src/parameters.cpp
    ./src/plugin.cpp
)

list(APPEND SRC_SHARED
	../../shared/noise_generator.h
	../../shared/noise_generator.cpp
)

list(APPEND SRC
    ${SRC_HEADERS}
    ${SRC_CPP}
    ${SRC_PARAMETERS}
    ${SRC_SHARED}
)

list(APPEND RES
	./res/icon.png
)

source_group(TREE ${CMAKE_CURRENT_LIST_DIR}/src PREFIX headers FILES ${SRC_HEADERS})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR}/src PREFIX src FILES ${SRC_CPP})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR}/src/parameters PREFIX src/parameters FILES ${SRC_PARAMETERS})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR}/../../shared PREFIX shared FILES ${SRC_SHARED})

blink_plugin_add_synth("test" "${SRC}" "${RES}")

target_include_directories(${PROJECT_NAME} PRIVATE
	${bleat_SOURCE_DIR}/include
	../..
)
