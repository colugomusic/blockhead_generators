cmake_minimum_required(VERSION 3.0)
project(synth_berk VERSION 0.5.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Modules)

include(CPM)
include(plugin)

list(APPEND SRC
    ./src/dsp.hpp
    ./src/glottis.h
    ./src/glottis.cpp
    ./src/model.h
    ./src/plugin.cpp
    ./src/simplexnoise1234.h
    ./src/simplexnoise1234.cpp
	../../shared/resampler.h
	../../shared/tract_params.hpp
)

list(APPEND RES
	./res/icon.png
)

blink_plugin_add_synth("berk" "${SRC}" "${RES}")

target_include_directories(${PROJECT_NAME} PRIVATE
	../..
)
