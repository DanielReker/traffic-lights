find_package(SFML COMPONENTS system window graphics CONFIG)


if (SFML_FOUND)
	file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
	file(GLOB SOURCE_LIST CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

	add_executable(traffic-lights-sfml ${HEADER_LIST} ${SOURCE_LIST})

	target_compile_features(traffic-lights-sfml PRIVATE cxx_std_23)
	set_target_properties(traffic-lights-sfml PROPERTIES
		CXX_STANDARD_REQUIRED ON
		CXX_EXTENSIONS OFF
	)

	source_group(
		TREE "${CMAKE_CURRENT_SOURCE_DIR}"
		PREFIX "SFML app headers"
		FILES ${HEADER_LIST}
	)

	add_custom_target(copy-assets-sfml
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets/ ${CMAKE_CURRENT_BINARY_DIR}/assets
	)

	add_dependencies(traffic-lights-sfml copy-assets-sfml)

	target_link_libraries(traffic-lights-sfml PRIVATE
		traffic-lights-model
		sfml-system
		sfml-window
		sfml-graphics
	)

else()
	message(STATUS "SFML not found, skipping Traffic Lights SFML application")
endif()


