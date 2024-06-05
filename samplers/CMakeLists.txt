cmake_minimum_required(VERSION 3.0)
project(samplers)

add_subdirectory(classic)
add_subdirectory(fudge)

cpm_export_variables(blink)