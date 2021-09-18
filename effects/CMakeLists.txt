cmake_minimum_required(VERSION 3.0)
project(effects)

add_subdirectory(filter_bandpass)
add_subdirectory(filter_highpass)
add_subdirectory(filter_lowpass)
add_subdirectory(freeze)
add_subdirectory(lofi)
add_subdirectory(ring_modulator)
add_subdirectory(spring_reverb)
add_subdirectory(tract)

cpm_export_variables(blink)