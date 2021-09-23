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

function(blink_plugin_get_base_filename out_var name type)
	set(${out_var} ${type}.${name}.v${PROJECT_VERSION}.${PLATFORM_DIR}$<${debug_build}:.dbg> PARENT_SCOPE)
endfunction()

function(blink_plugin_get_target_name out_var name type)
	set(${out_var} ${type}_${name} PARENT_SCOPE)
endfunction()

function(blink_plugin_add name type src)
	blink_plugin_get_base_filename(output_name ${name} ${type})
	blink_plugin_get_target_name(target_name ${name} ${type})

	add_library(${target_name} SHARED ${src})
	target_link_libraries(${target_name} PRIVATE blink_plugin)

	set_target_properties(${target_name} PROPERTIES
		OUTPUT_NAME ${output_name}
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

	target_compile_definitions(${target_name} PRIVATE PLUGIN_VERSION="${PROJECT_VERSION}")
endfunction()

function(blink_plugin_add_effect name src)
	blink_plugin_add(${name} "effect" "${src}")
endfunction()

function(blink_plugin_add_sampler name src)
	blink_plugin_add(${name} "sampler" "${src}")
endfunction()

function(blink_plugin_add_synth name src)
	blink_plugin_add(${name} "synth" "${src}")
endfunction()

function(blink_plugin_add_package name type src resources)
	blink_plugin_get_base_filename(plugin_base_filename ${name} ${type})
	blink_plugin_get_target_name(target_name ${name} ${type})

	add_library(${target_name} SHARED ${src} ${resources})
	target_link_libraries(${target_name} PRIVATE blink_plugin)

	set_target_properties(${target_name} PROPERTIES
		OUTPUT_NAME ${plugin_base_filename}
		FOLDER "plugins/${type}s"
		PREFIX ""
		SUFFIX ".blink_plugin"
		CXX_STANDARD 17
	)

	target_compile_definitions(${target_name} PRIVATE PLUGIN_VERSION="${PROJECT_VERSION}")

	set(tar_path $<TARGET_FILE_DIR:${target_name}>/${plugin_base_filename}.tar)
	set(blink_path ${PLUGIN_OUTPUT_DIR}/${plugin_base_filename}.blink)

	foreach(resource ${resources})
		get_filename_component(filename ${resource} NAME)

		add_custom_command(
			TARGET ${target_name}
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${resource} $<TARGET_FILE_DIR:${target_name}>/res/${filename}
		)

		list(APPEND resource_files $<TARGET_FILE_DIR:${target_name}>/res/${filename})
	endforeach()

	add_custom_command(
		TARGET ${target_name}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E tar c ${tar_path} -- $<TARGET_FILE:${target_name}> ${resource_files}
		WORKING_DIRECTORY $<TARGET_FILE_DIR:${target_name}>
	)

	add_custom_command(
		TARGET ${target_name}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ${tar_path} ${blink_path}
	)
endfunction()

function(blink_plugin_add_effect_package name src resources)
	blink_plugin_add_package(${name} "effect" "${src}" "${resources}")
endfunction()

function(blink_plugin_add_sampler_package name src resources)
	blink_plugin_add_package(${name} "sampler" "${src}" "${resources}")
endfunction()

function(blink_plugin_add_synth_package name src resources)
	blink_plugin_add_package(${name} "synth" "${src}" "${resources}")
endfunction()