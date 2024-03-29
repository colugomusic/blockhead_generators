cmake_minimum_required(VERSION 3.0)
project(effect_saturator VERSION 0.4.1)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Modules)

include(CPM)
include(blink_plugin)

CPMAddPackage("gh:colugomusic/snd#master")
cpm_export_variables(snd)

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
    ./src/parameters/drive.hpp
    ./src/parameters/mix.hpp
    ./src/plugin.h
    ./src/plugin.cpp
)

list(APPEND RES
	./res/icon.png
)

blink_plugin_add_effect("saturator" "${SRC}" "${RES}")

target_include_directories(${PROJECT_NAME} PRIVATE
	${snd_SOURCE_DIR}/include
	../..
)
