cmake_minimum_required(VERSION 3.20)
project(effect_tract VERSION 0.5.1)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Modules)

include(CPM)
include(plugin)

list(APPEND SRC
  ./src/dsp.hpp
  ./src/model.h
  ./src/plugin.cpp
	../../shared/tract_params.hpp
)

list(APPEND RES
	./res/icon.png
)

blink_plugin_add_effect("tract" "${SRC}" "${RES}")

target_include_directories(${PROJECT_NAME} PRIVATE
	../..
)
