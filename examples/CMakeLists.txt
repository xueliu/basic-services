foreach(EXAMPLE ex-thread-pool ex-queue ex-timestamp ex-source-file ex-log ex-serial ex-circle-buffer)

	add_executable(${EXAMPLE} "")

	target_sources(
		${EXAMPLE}
		PRIVATE
		${CMAKE_CURRENT_LIST_DIR}/${EXAMPLE}.cpp
	)

	target_include_directories(
		${EXAMPLE}
		PRIVATE
#		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
		$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include> # using the top-most directory of the source tree
#		$<INSTALL_INTERFACE:include>
		${CMAKE_BINARY_DIR} # using the top-most directory of the build tree
	)

	target_link_libraries(
		${EXAMPLE}
		PRIVATE
		basic-services
	)

	install (
		TARGETS ${EXAMPLE}
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)

endforeach()