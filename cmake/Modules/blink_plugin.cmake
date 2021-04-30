set(PLUGIN_OUTPUT_DIR bin CACHE PATH "plugin output directory")

set(on_windows $<STREQUAL:${CMAKE_SYSTEM_NAME},Windows>)
set(on_macos $<STREQUAL:${CMAKE_SYSTEM_NAME},Darwin>)
set(on_linux $<STREQUAL:${CMAKE_SYSTEM_NAME},Linux>)
set(PLATFORM_DIR $<IF:${on_windows},win64,$<IF:${on_macos},macos,x11>>)

CPMAddPackage("gh:colugomusic/blink#master")

CPMAddPackage(
    NAME madronalib
    GITHUB_REPOSITORY madronalabs/madronalib
    GIT_TAG master
    DOWNLOAD_ONLY YES
)

cpm_export_variables(blink)

function(blink_plugin_set_target_properties name type)
	set_target_properties(${PROJECT_NAME} PROPERTIES
		OUTPUT_NAME ${type}.${name}.${PLATFORM_DIR}
		FOLDER "plugins/${type}s"
		SUFFIX ".blink"
		RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PLUGIN_OUTPUT_DIR}
		RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PLUGIN_OUTPUT_DIR}
		RUNTIME_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR}
		LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PLUGIN_OUTPUT_DIR}
		LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PLUGIN_OUTPUT_DIR}
		LIBRARY_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR}
		CXX_STANDARD 17
	)
endfunction()

function(blink_plugin_set_effect_target_properties name)
	blink_plugin_set_target_properties(${name} "effect")
endfunction()

function(blink_plugin_set_sampler_target_properties name)
	blink_plugin_set_target_properties(${name} "sampler")
endfunction()

function(blink_plugin_set_synth_target_properties name)
	blink_plugin_set_target_properties(${name} "synth")
endfunction()
