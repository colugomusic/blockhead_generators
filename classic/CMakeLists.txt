cmake_minimum_required(VERSION 3.0)
project(generator_classic)

set(BLKHDGEN_ROOT ${CMAKE_CURRENT_LIST_DIR}/../../blkhdgen CACHE PATH "blkhdgen root directory")
set(PLUGIN_OUTPUT_DIR bin CACHE PATH "plugin output directory")

set(on_windows $<STREQUAL:${CMAKE_SYSTEM_NAME},Windows>)
set(on_macos $<STREQUAL:${CMAKE_SYSTEM_NAME},Darwin>)
set(on_linux $<STREQUAL:${CMAKE_SYSTEM_NAME},Linux>)
set(PLATFORM_DIR $<IF:${on_windows},win64,$<IF:${on_macos},macos,x11>>)

include(FetchContent)

set(FETCHCONTENT_QUIET off)

FetchContent_Declare(
	madronalib
	GIT_REPOSITORY https://github.com/madronalabs/madronalib.git
)

FetchContent_Populate(madronalib)

list(APPEND SRC
    ./src/plugin.cpp
)

add_library(${PROJECT_NAME} SHARED ${SRC})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${BLKHDGEN_ROOT}/include
    ${BLKHDGEN_ROOT}/plugin
	${madronalib_SOURCE_DIR}/source
)

set_target_properties(${PROJECT_NAME} PROPERTIES
	OUTPUT_NAME classic.${PLATFORM_DIR}
	SUFFIX ".blkhdgen"
	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PLUGIN_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PLUGIN_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PLUGIN_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PLUGIN_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR}
    CXX_STANDARD 17
)