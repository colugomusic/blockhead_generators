set(PLUGIN_OUTPUT_DIR bin CACHE PATH "plugin output directory")

set(on_windows $<STREQUAL:${CMAKE_SYSTEM_NAME},Windows>)
set(on_macos $<STREQUAL:${CMAKE_SYSTEM_NAME},Darwin>)
set(on_linux $<STREQUAL:${CMAKE_SYSTEM_NAME},Linux>)
set(PLATFORM_DIR $<IF:${on_windows},win64,$<IF:${on_macos},macos,linux>>)

CPMAddPackage(
	NAME blink
	GITHUB_REPOSITORY colugomusic/blink
	GIT_TAG master
	DOWNLOAD_ONLY YES
)

if (NOT TARGET blink_plugin)
	add_subdirectory(${blink_SOURCE_DIR}/plugin/blink blink_plugin)
endif()

function(blink_plugin_set_target_properties name type)
	set_target_properties(${PROJECT_NAME} PROPERTIES
		OUTPUT_NAME ${type}.${name}.v${PROJECT_VERSION}.${PLATFORM_DIR}
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
	target_compile_definitions(${PROJECT_NAME} PRIVATE
		PLUGIN_VERSION="${PROJECT_VERSION}"
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

function(blink_plugin_add name src)
	add_library(${name} SHARED ${src})
	target_link_libraries(${name} PRIVATE blink_plugin)
endfunction()
