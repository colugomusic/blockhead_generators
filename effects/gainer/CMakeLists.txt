cmake_minimum_required(VERSION 3.20)
project(effect_gainer VERSION 0.3.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Modules)

include(plugin)

list(APPEND SRC
    ./src/dsp.hpp
    ./src/model.h
    ./src/plugin.cpp
)

list(APPEND RES
    ./res/icon.png
)

blink_plugin_add_effect("gainer" "${SRC}" "${RES}")

target_include_directories(effect_gainer PRIVATE
	../..
)
