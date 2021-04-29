cmake_minimum_required(VERSION 3.0)
project(sampler_classic)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Modules)

include(CPM)

set(PLUGIN_OUTPUT_DIR bin CACHE PATH "plugin output directory")

set(on_windows $<STREQUAL:${CMAKE_SYSTEM_NAME},Windows>)
set(on_macos $<STREQUAL:${CMAKE_SYSTEM_NAME},Darwin>)
set(on_linux $<STREQUAL:${CMAKE_SYSTEM_NAME},Linux>)
set(PLATFORM_DIR $<IF:${on_windows},win64,$<IF:${on_macos},macos,x11>>)

CPMAddPackage("gh:colugomusic/blink#master")
CPMAddPackage("gh:madronalabs/madronalib#master")

cpm_export_variables(blink)

list(APPEND SRC
    ./src/audio.h
    ./src/audio.cpp
    ./src/gui.h
    ./src/gui.cpp
    ./src/plugin.h
    ./src/plugin.cpp
	../../shared/noise_generator.h
	../../shared/noise_generator.cpp
)

add_library(${PROJECT_NAME} SHARED ${SRC})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${blink_SOURCE_DIR}/include
    ${blink_SOURCE_DIR}/plugin
	${madronalib_SOURCE_DIR}/source
    ../..
)

set_target_properties(${PROJECT_NAME} PROPERTIES
	OUTPUT_NAME sampler.classic.${PLATFORM_DIR}
	SUFFIX ".blink"
	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PLUGIN_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PLUGIN_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PLUGIN_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PLUGIN_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR}
    CXX_STANDARD 17
)