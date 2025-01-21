cmake_minimum_required(VERSION 3.10)

project(traffic-lights)

option(BUILD_NCURSES_APP "Build Traffic Lights NCurses application" ON)
option(BUILD_SFML_APP "Build Traffic Lights SFML application" ON)


find_package(boost_algorithm REQUIRED CONFIG)
find_package(boost_locale REQUIRED CONFIG)
find_package(boost_json REQUIRED CONFIG)
find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)


add_subdirectory(model)

if(BUILD_NCURSES_APP)
	add_subdirectory(ncurses-app)
endif()

if(BUILD_SFML_APP)
	add_subdirectory(sfml-app)
endif()

