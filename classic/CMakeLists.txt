cmake_minimum_required(VERSION 3.0)
project(generator_classic)

set(BLKHDGEN_ROOT ${CMAKE_CURRENT_LIST_DIR}/../../blkhdgen CACHE PATH "blkhdgen root directory")

list(APPEND SRC
    ./src/plugin.cpp
)

add_library(${PROJECT_NAME} STATIC ${SRC})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${BLKHDGEN_ROOT}/include
    ${BLKHDGEN_ROOT}/plugin
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
)