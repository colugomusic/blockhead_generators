cmake_minimum_required(VERSION 3.0)
project(effect_tract VERSION 0.4.1)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Modules)

include(CPM)
include(blink_lib)

list(APPEND SRC
    ./src/audio.h
    ./src/audio.cpp
    ./src/audio_data.h
    ./src/audio_data.cpp
    ./src/instance.h
    ./src/instance.cpp
    ./src/parameters.h
    ./src/parameters.cpp
    ./src/plugin.h
    ./src/plugin.cpp
	../../shared/resampler.h
	../../shared/tract.h
	../../shared/tract.cpp
	../../shared/tract_parameters/quality.hpp
	../../shared/tract_parameters/throat_diameter.hpp
	../../shared/tract_parameters/throat_index.hpp
	../../shared/tract_parameters/tongue_diameter.hpp
	../../shared/tract_parameters/tongue_index.hpp
)

list(APPEND RES
	./res/icon.png
)

blink_plugin_add_effect("tract" "${SRC}" "${RES}")

target_include_directories(${PROJECT_NAME} PRIVATE
	../..
)
