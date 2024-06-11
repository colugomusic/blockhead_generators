include(CPM)

CPMAddPackage(
	NAME              blink
	GITHUB_REPOSITORY colugomusic/blink
	GIT_TAG           master
	DOWNLOAD_ONLY     YES
)

if (NOT TARGET blink_plugin)
  add_subdirectory(${blink_SOURCE_DIR}/lib/plugin blink_plugin)
endif()

source_group(src REGULAR_EXPRESSION "/src/.*\\.(h|cpp)")