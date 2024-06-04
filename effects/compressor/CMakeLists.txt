cmake_minimum_required(VERSION 3.0)
project(effect_compressor VERSION 0.4.2)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Modules)

include(blink_lib)

CPMAddPackage("gh:colugomusic/snd#master")
cpm_export_variables(snd)

list(APPEND SRC
	./src/dsp.hpp
	./src/model.h
	./src/plugin.h
	./src/plugin.cpp
)

list(APPEND RES
	./res/icon.png
)

blink_plugin_add_effect("compressor" "${SRC}" "${RES}")

target_include_directories(effect_compressor PRIVATE
	../..
)
