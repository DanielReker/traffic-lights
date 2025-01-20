set(CURSES_NEED_NCURSES TRUE)
set(CURSES_NEED_WIDE TRUE)
find_package(Curses)

find_package(unofficial-pdcurses CONFIG)


if (Curses_FOUND OR unofficial-pdcurses_FOUND)
	file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
	file(GLOB SOURCE_LIST CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

	add_executable(traffic-lights-ncurses ${HEADER_LIST} ${SOURCE_LIST})

	target_compile_features(traffic-lights-ncurses PRIVATE cxx_std_23)
	set_target_properties(traffic-lights-ncurses PROPERTIES
		CXX_STANDARD_REQUIRED ON
		CXX_EXTENSIONS OFF
	)

	source_group(
		TREE "${CMAKE_CURRENT_SOURCE_DIR}"
		PREFIX "NCurses app headers"
		FILES ${HEADER_LIST}
	)

	add_custom_target(copy-assets-ncurses
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets/ ${CMAKE_CURRENT_BINARY_DIR}/assets
	)

	add_dependencies(traffic-lights-ncurses copy-assets-ncurses)

	find_package(ICU REQUIRED COMPONENTS i18n uc data)

	target_link_libraries(traffic-lights-ncurses PRIVATE
		traffic-lights-model
		Boost::algorithm
		Boost::locale
		Boost::json
		ICU::i18n
		ICU::uc
		ICU::data
		spdlog::spdlog
		fmt::fmt
	)



	if (Curses_FOUND)
		target_include_directories(traffic-lights-ncurses PRIVATE ${CURSES_INCLUDE_DIRS})
		target_compile_options(traffic-lights-ncurses PRIVATE ${CURSES_CFLAGS})
		#target_compile_options(traffic-lights-ncurses PRIVATE "-DNCURSES_STATIC")
		target_link_libraries(traffic-lights-ncurses PRIVATE ${CURSES_LIBRARIES})

		target_compile_definitions(traffic-lights-ncurses PRIVATE NCURSES)
	elseif(unofficial-pdcurses_FOUND)
		target_link_libraries(traffic-lights-ncurses PRIVATE unofficial::pdcurses::pdcurses)

		target_compile_definitions(traffic-lights-ncurses PRIVATE PDCURSES)
	endif()

else()
	message(STATUS "NCurses/PDCurses not found, skipping Traffic Lights NCurses application")
endif()


