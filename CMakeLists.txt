cmake_minimum_required(VERSION 3.0)
project(blockhead_generators)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_OSX_ARCHITECTURES arm64;x86_64)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.11)

add_subdirectory(effects)
add_subdirectory(samplers)
add_subdirectory(synths)
