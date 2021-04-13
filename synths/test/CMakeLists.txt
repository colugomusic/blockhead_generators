cmake_minimum_required(VERSION 3.0)
project(synth_test)

set(BLINK_ROOT ${CMAKE_CURRENT_LIST_DIR}/../../blink CACHE PATH "blink root directory")
set(PLUGIN_OUTPUT_DIR bin CACHE PATH "plugin output directory")
set(MADRONALIB_DIR "" CACHE PATH "madronalib directory")
set(BLEAT_DIR "" CACHE PATH "bleat directory")

set(on_windows $<STREQUAL:${CMAKE_SYSTEM_NAME},Windows>)
set(on_macos $<STREQUAL:${CMAKE_SYSTEM_NAME},Darwin>)
set(on_linux $<STREQUAL:${CMAKE_SYSTEM_NAME},Linux>)
set(PLATFORM_DIR $<IF:${on_windows},win64,$<IF:${on_macos},macos,x11>>)

include(FetchContent)
set(FETCHCONTENT_QUIET off)

if (MADRONALIB_DIR STREQUAL "")
	FetchContent_Declare(
		madronalib
		GIT_REPOSITORY https://github.com/madronalabs/madronalib.git
	)
	FetchContent_Populate(madronalib)
	set(MADRONALIB_DIR ${madronalib_SOURCE_DIR})
endif()

if (BLEAT_DIR STREQUAL "")
	FetchContent_Declare(
		bleat
		GIT_REPOSITORY https://github.com/colugomusic/bleat.git
	)
	FetchContent_Populate(bleat)
	set(BLEAT_DIR ${bleat_SOURCE_DIR})
endif()

list(APPEND SRC
    ./src/audio.h
    ./src/audio.cpp
    ./src/plugin.h
    ./src/plugin.cpp
)

add_library(${PROJECT_NAME} SHARED ${SRC})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${BLINK_ROOT}/include
    ${BLINK_ROOT}/plugin
	${MADRONALIB_DIR}/source
	${BLEAT_DIR}/include
)

set_target_properties(${PROJECT_NAME} PROPERTIES
	OUTPUT_NAME synth.test.${PLATFORM_DIR}
	SUFFIX ".blink"
	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PLUGIN_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PLUGIN_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PLUGIN_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PLUGIN_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR}
    CXX_STANDARD 17
)