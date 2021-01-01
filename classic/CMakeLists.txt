cmake_minimum_required(VERSION 3.0)
project(generator_classic)

set(BLKHDGEN_ROOT ${CMAKE_CURRENT_LIST_DIR}/../../blkhdgen CACHE PATH "blkhdgen root directory")

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

add_library(${PROJECT_NAME} STATIC ${SRC})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${BLKHDGEN_ROOT}/include
    ${BLKHDGEN_ROOT}/plugin
	${madronalib_SOURCE_DIR}/source
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
)