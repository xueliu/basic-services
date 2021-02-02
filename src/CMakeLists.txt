set(LIB_NAME ${CMAKE_PROJECT_NAME})

add_library(${LIB_NAME} "")
generate_export_header(${LIB_NAME})

set(
	${LIB_NAME}_PUBLIC_HEADERS
	${CMAKE_SOURCE_DIR}/include/version.h
	${CMAKE_SOURCE_DIR}/include/timer.h
	${CMAKE_SOURCE_DIR}/include/thread.h
	${CMAKE_SOURCE_DIR}/include/thread-pool.h
)

target_sources(
	${LIB_NAME}
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/current-thread.cpp
	${CMAKE_CURRENT_LIST_DIR}/logging.cpp
	${CMAKE_CURRENT_LIST_DIR}/log-stream.cpp
	${CMAKE_CURRENT_LIST_DIR}/timer.cpp
	${CMAKE_CURRENT_LIST_DIR}/serial-device.cpp
	${CMAKE_CURRENT_LIST_DIR}/serial-buffer-device.cpp
	${CMAKE_CURRENT_LIST_DIR}/serial-packet-device.cpp
	${CMAKE_CURRENT_LIST_DIR}/thread.cpp
	${CMAKE_CURRENT_LIST_DIR}/thread-pool.cpp
	${CMAKE_CURRENT_LIST_DIR}/timestamp.cpp
)


if(WIN32)
	target_sources(
		${LIB_NAME}
		PRIVATE
		${CMAKE_CURRENT_LIST_DIR}/serial-device-wins.cpp
	)
elseif(UNIX)
	target_sources(
		${LIB_NAME}
		PRIVATE
		${CMAKE_CURRENT_LIST_DIR}/serial-device-unix.cpp
	)
endif()

target_include_directories(
	${LIB_NAME}
	PRIVATE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/impl>
	$<INSTALL_INTERFACE:include>
	${CMAKE_CURRENT_BINARY_DIR}
)

if(WIN32)
	target_include_directories(
		${LIB_NAME}
		PRIVATE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/impl/wins>
	)
elseif(UNIX)
	target_include_directories(
		${LIB_NAME}
		PRIVATE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/impl/unix>
	)
endif()

target_link_libraries(
	${LIB_NAME}
	PUBLIC
	Threads::Threads
)

configure_file(${CMAKE_CURRENT_LIST_DIR}/${LIB_NAME}_version.h.in "${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}_version.h" @ONLY)

set_target_properties(${LIB_NAME} PROPERTIES VERSION    ${${CMAKE_PROJECT_NAME}_VERSION_SHORT})
set_target_properties(${LIB_NAME} PROPERTIES SOVERSION  ${${CMAKE_PROJECT_NAME}_VERSION_SHORT})
set_target_properties(${LIB_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)
set_target_properties(${LIB_NAME} PROPERTIES VISIBILITY_INLINES_HIDDEN 1)
set_target_properties(
	${LIB_NAME}
	PROPERTIES PUBLIC_HEADER
	"${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}_version.h;${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}_export.h;${${LIB_NAME}_PUBLIC_HEADERS}"
)

install(
	TARGETS ${LIB_NAME}
	EXPORT  ${LIB_NAME}_targets
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT shlib
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${LIB_NAME}
	INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${LIB_NAME}
)

# -- write cmake package config file
write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}/${LIB_NAME}-config-version.cmake"
	VERSION ${${CMAKE_PROJECT_NAME}_VERSION_SHORT}
	COMPATIBILITY AnyNewerVersion
)

export(
	EXPORT ${LIB_NAME}_targets
	FILE "${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}/${LIB_NAME}-targets.cmake"
	NAMESPACE lx::
)

configure_file(
	cmake/${LIB_NAME}-config.cmake
	"${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}/${LIB_NAME}-config.cmake"
	COPYONLY
)

set(ConfigPackageLocation lib/cmake/${LIB_NAME})
install(
	EXPORT ${LIB_NAME}_targets
	FILE ${LIB_NAME}-targets.cmake
	NAMESPACE lx::
	DESTINATION ${ConfigPackageLocation}
)

install(
	FILES  cmake/${LIB_NAME}-config.cmake
	"${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}/${LIB_NAME}-config-version.cmake"
	DESTINATION ${ConfigPackageLocation}
	COMPONENT Devel
)