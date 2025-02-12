if (NOT TARGET blink_plugin)
	add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../extern/submodules/blink/lib/plugin blink_plugin)
endif()

source_group(src REGULAR_EXPRESSION "/src/.*\\.(h|cpp)")