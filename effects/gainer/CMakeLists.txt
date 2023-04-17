cmake_minimum_required(VERSION 3.0)
project(effect_gainer VERSION 0.2.1)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Modules)

include(blink_plugin)

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
	./src/parameters/pan.hpp
    ./src/plugin.h
    ./src/plugin.cpp
)

list(APPEND RES
    ./res/icon.png
)

blink_plugin_add_effect("gainer" "${SRC}" "${RES}")

target_include_directories(effect_gainer PRIVATE
	../..
)
