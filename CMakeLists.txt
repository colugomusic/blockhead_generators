cmake_minimum_required(VERSION 3.0)
project(blockhead_generators)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_OSX_ARCHITECTURES arm64;x86_64)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.11)

add_subdirectory(effects)
add_subdirectory(samplers)
add_subdirectory(synths)

add_custom_target(all_plugins)
add_dependencies(all_plugins
	effect_compressor
	effect_filter.bandpass
	effect_filter.highpass
	effect_filter.lowpass
	#effect_freeze
	#effect_gainer
	#effect_lofi
	#effect_rattler
	#effect_resonator
	#effect_ring_modulator
	#effect_saturator
	#effect_spring_reverb
	#effect_tract
	#effect_wavebender
	#effect_zap
	sampler_classic
	sampler_fudge
	synth_berk
	synth_test
)
