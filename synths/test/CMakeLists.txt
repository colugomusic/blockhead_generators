cmake_minimum_required(VERSION 3.0)
project(synth_test VERSION 0.4.3)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Modules)

include(CPM)
include(plugin)

CPMAddPackage("gh:colugomusic/bleat#master")

list(APPEND SRC
	./src/dsp.hpp
	./src/model.h
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
