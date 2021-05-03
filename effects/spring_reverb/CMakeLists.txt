cmake_minimum_required(VERSION 3.0)
project(effect_spring_reverb VERSION 0.1.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Modules)

include(CPM)
include(blink_plugin)

list(APPEND SRC
    ./src/audio.h
    ./src/audio.cpp
    ./src/plugin.h
    ./src/plugin.cpp
)

add_library(${PROJECT_NAME} SHARED ${SRC})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${blink_SOURCE_DIR}/include
    ${blink_SOURCE_DIR}/plugin
	${madronalib_SOURCE_DIR}/source
)

blink_plugin_set_effect_target_properties("spring_reverb")