set(PLUGIN_OUTPUT_DIR bin CACHE PATH "plugin output directory")

set(on_windows $<STREQUAL:${CMAKE_SYSTEM_NAME},Windows>)
set(on_macos $<STREQUAL:${CMAKE_SYSTEM_NAME},Darwin>)
set(on_linux $<STREQUAL:${CMAKE_SYSTEM_NAME},Linux>)
set(debug_build $<OR:$<CONFIG:Debug>,$<STREQUAL:${CMAKE_BUILD_TYPE},Debug}>>)
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
		OUTPUT_NAME ${type}.${name}.v${PROJECT_VERSION}.${PLATFORM_DIR}$<${debug_build}:.dbg>
		FOLDER "plugins/${type}s"
		PREFIX ""
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

function(blink_plugin_add_2 name type src)
	add_library(${type}_${name} SHARED ${src})
	target_link_libraries(${type}_${name} PRIVATE blink_plugin)

	set_target_properties(${type}_${name} PROPERTIES
		OUTPUT_NAME ${type}.${name}.v${PROJECT_VERSION}.${PLATFORM_DIR}$<${debug_build}:.dbg>
		FOLDER "plugins/${type}s"
		PREFIX ""
		SUFFIX ".blink_plugin"
		RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PLUGIN_OUTPUT_DIR}
		RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PLUGIN_OUTPUT_DIR}
		RUNTIME_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR}
		LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PLUGIN_OUTPUT_DIR}
		LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PLUGIN_OUTPUT_DIR}
		LIBRARY_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR}
		CXX_STANDARD 17
	)

	target_compile_definitions(${type}_${name} PRIVATE PLUGIN_VERSION="${PROJECT_VERSION}")

	set(tar_path ${type}.${name}.v${PROJECT_VERSION}.${PLATFORM_DIR}$<${debug_build}:.dbg>.tar)
	set(blink_path ${type}.${name}.v${PROJECT_VERSION}.${PLATFORM_DIR}$<${debug_build}:.dbg>.blink)
	
	add_custom_command(
		TARGET ${type}_${name}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E tar c ${tar_path} -- $<TARGET_FILE:${type}_${name}>
		WORKING_DIRECTORY $<TARGET_FILE_DIR:${type}_${name}>
	)

	add_custom_command(
		TARGET ${type}_${name}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E rename ${tar_path} ${blink_path}
		WORKING_DIRECTORY $<TARGET_FILE_DIR:${type}_${name}>
	)
endfunction()

function(blink_plugin_add_effect name src)
	blink_plugin_add_2(${name} "effect" "${src}")
endfunction()