cmake_minimum_required(VERSION 3.0)
project(synths)

add_subdirectory(berk)
add_subdirectory(test)

cpm_export_variables(bleat)
cpm_export_variables(blink)