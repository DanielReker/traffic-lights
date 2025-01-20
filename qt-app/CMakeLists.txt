find_package(Qt6Widgets CONFIG REQUIRED)


if (Qt6Widgets_FOUND)
	qt_standard_project_setup()

	qt_add_executable(traffic-lights-qt
		MyWindow.cpp
		MyWindow.hpp
		main.cpp
	)

	target_link_libraries(traffic-lights-qt PRIVATE
		Qt6::Widgets
		traffic-lights-model
	)

	target_compile_features(traffic-lights-qt PRIVATE cxx_std_23)
	set_target_properties(traffic-lights-qt PROPERTIES
		CXX_STANDARD_REQUIRED ON
		CXX_EXTENSIONS OFF
	)

	set_target_properties(traffic-lights-qt PROPERTIES
		WIN32_EXECUTABLE ON
		MACOSX_BUNDLE ON
	)

	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		find_program(TOOL_WINDEPLOYQT NAMES windeployqt.debug.bat)
	else()
		find_program(TOOL_WINDEPLOYQT NAMES windeployqt)
	endif()

	add_custom_command(TARGET traffic-lights-qt POST_BUILD
		COMMAND ${TOOL_WINDEPLOYQT}
				$<TARGET_FILE:traffic-lights-qt>
		COMMENT "Running windeployqt..."
	)


else()
	message(STATUS "Qt not found, skipping Traffic Lights Qt application")
endif()


