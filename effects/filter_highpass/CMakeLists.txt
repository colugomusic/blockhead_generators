cmake_minimum_required(VERSION 3.0)
project(effect_filter_highpass VERSION 0.1.2)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Modules)

include(CPM)
include(blink_plugin)

list(APPEND SRC
    ./src/audio.h
    ./src/audio.cpp
    ./src/instance.h
    ./src/instance.cpp
    ./src/plugin.h
    ./src/plugin.cpp
)

blink_plugin_add(${PROJECT_NAME} "${SRC}")
blink_plugin_set_effect_target_properties("filter.highpass")
