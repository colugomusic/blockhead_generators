cmake_minimum_required(VERSION 3.0)
project(effect_freeze VERSION 0.2.1)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Modules)

include(CPM)
include(blink_plugin)

list(APPEND SRC
    ./src/audio.h
    ./src/audio_data.h
    ./src/audio.cpp
    ./src/buffer.h
    ./src/controller.h
    ./src/controller.cpp
    ./src/grain.h
    ./src/instance.h
    ./src/instance.cpp
    ./src/particle.h
    ./src/particle.cpp
    ./src/plugin.h
    ./src/plugin.cpp
)

blink_plugin_add(${PROJECT_NAME} "${SRC}")
blink_plugin_set_effect_target_properties("freeze")
