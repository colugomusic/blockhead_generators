cmake_minimum_required(VERSION 3.0)
project(synth_berk VERSION 0.3.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Modules)

include(CPM)
include(blink_plugin)

CPMAddPackage("gh:colugomusic/bleat#master")
cpm_export_variables(bleat)

list(APPEND SRC
    ./src/audio.h
    ./src/audio.cpp
    ./src/audio_data.h
    ./src/audio_data.cpp
    ./src/glottis.h
    ./src/glottis.cpp
    ./src/instance.h
    ./src/instance.cpp
    ./src/parameters.h
    ./src/parameters.cpp
    ./src/parameters/amp.hpp
    ./src/parameters/buzz.hpp
    ./src/parameters/pan.hpp
    ./src/parameters/pitch.hpp
    ./src/plugin.h
    ./src/plugin.cpp
    ./src/simplexnoise1234.h
    ./src/simplexnoise1234.cpp
	../../shared/resampler.h
	../../shared/tract.h
	../../shared/tract.cpp
	../../shared/tract_parameters/fricative_intensity.hpp
	../../shared/tract_parameters/quality.hpp
	../../shared/tract_parameters/throat_diameter.hpp
	../../shared/tract_parameters/throat_index.hpp
	../../shared/tract_parameters/tongue_diameter.hpp
	../../shared/tract_parameters/tongue_index.hpp
)

list(APPEND RES
	./res/icon.png
)

blink_plugin_add_synth("berk" "${SRC}" "${RES}")

target_include_directories(${PROJECT_NAME} PRIVATE
	../..
)
