file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

add_library(traffic-lights-model STATIC stub.cpp ${HEADER_LIST})
target_include_directories(traffic-lights-model PUBLIC "./include/")

target_compile_features(traffic-lights-model PRIVATE cxx_std_23)
set_target_properties(traffic-lights-model PROPERTIES
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
)

source_group(
	TREE "${CMAKE_CURRENT_SOURCE_DIR}/include"
	PREFIX "Model headers"
	FILES ${HEADER_LIST}
)

target_link_libraries(traffic-lights-model PUBLIC
	Boost::json
	spdlog::spdlog
)