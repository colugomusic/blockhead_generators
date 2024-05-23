cmake_minimum_required(VERSION 3.0)
project(effect_freeze VERSION 0.5.2)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Modules)

include(CPM)
include(blink_lib)

list(APPEND SRC
    ./src/audio.h
    ./src/audio_data.h
    ./src/audio_data.cpp
    ./src/audio.cpp
    ./src/buffer.h
    ./src/controller.h
    ./src/controller.cpp
    ./src/grain.h
    ./src/instance.h
    ./src/instance.cpp
    ./src/parameters.h
    ./src/parameters.cpp
    ./src/particle.h
    ./src/particle.cpp
    ./src/plugin.h
    ./src/plugin.cpp
)

list(APPEND RES
    ./res/icon.png
)

blink_plugin_add_effect("freeze" "${SRC}" "${RES}")
