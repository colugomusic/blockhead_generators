cmake_minimum_required(VERSION 3.20)
project(effect_freeze VERSION 0.6.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../cmake/Modules)

include(plugin)

list(APPEND SRC
	./src/dsp.hpp
	./src/model.h
	./src/buffer.h
	./src/plugin.cpp
)

list(APPEND RES
	./res/icon.png
)

blink_plugin_add_effect("freeze" "${SRC}" "${RES}")
