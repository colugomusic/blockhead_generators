cmake_minimum_required(VERSION 3.0)
project(effects)

add_subdirectory(filter)
add_subdirectory(freeze)
add_subdirectory(ring_modulator)
add_subdirectory(spring_reverb)

cpm_export_variables(blink)