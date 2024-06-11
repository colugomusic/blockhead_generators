cmake_minimum_required(VERSION 3.0)
project(effect_template VERSION 0.1.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Modules)

include(plugin)

list(APPEND SRC
	./src/model.h
	./src/dsp.hpp
	./src/plugin.cpp
)

list(APPEND RES
	./res/icon.png
)

blink_plugin_add_effect("template" "${SRC}" "${RES}")

target_include_directories(effect_template PRIVATE
	../..
)
