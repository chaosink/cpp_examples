cmake_minimum_required(VERSION 3.7)
project(cpp_example LANGUAGES CXX)

# compiling options
set(CMAKE_CXX_STANDARD 17)
if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Ox /openmp -d2SSAOptimizer-") # -d2SSAOptimizer- for MSVC bug
else()
	set(CXX_WARNINGS "-Wall -Wextra -Wpointer-arith -Wcast-align -fstrict-aliasing")
	if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
		set(CXX_WARNINGS "${CXX_WARNINGS} -Wno-maybe-uninitialized")
	endif()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_WARNINGS} -fvisibility-inlines-hidden")

	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -fopenmp")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX__FLAGS_RELEASE} -O3 -fopenmp")

	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
endif()

set(source_dir example)
set(common_head common.hpp)
set(source_files
	algorithm/a_star.cpp
	algorithm/Dijkstra.cpp
	algorithm/Floyd.cpp
	algorithm/Hungarian.cpp
	algorithm/Kruskal.cpp
	algorithm/Prim.cpp
	algorithm/quick_sort.cpp
	algorithm/SPFA.cpp

	data_structure/binary_indexed_tree.cpp
	data_structure/disjoint_sets.cpp
	data_structure/priority_queue.cpp
	data_structure/segment_tree.cpp
	data_structure/sparse_table.cpp
	data_structure/trie.cpp

	feature/class_size.cpp
	feature/comma_operator.cpp
	feature/infinite_nan.cpp
	feature/iterator.cpp
	feature/list_initialization.cpp
	feature/new_delete.cpp

	functional_programming/maybe_monad.cpp

	metaprogramming/binary.cpp
	metaprogramming/has_type_.cpp
	metaprogramming/policy/policy.cpp
	metaprogramming/sizes_of.cpp
	metaprogramming/short_circuit.cpp
	metaprogramming/VarTypeDict.cpp

	standard_library/stringstream_eof.cpp
	standard_library/wide_character.cpp
)

include_directories(${source_dir})
foreach(src ${source_files})
	get_filename_component(bin ${src} NAME_WE)
	string(REGEX MATCH "^[a-z_]*/" dir ${src})
	add_executable(${bin}
		${source_dir}/${common_head}
		${source_dir}/${src}
	)
	set_target_properties(${bin} PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${dir}"
	)
endforeach()
if(NOT MSVC)
	target_compile_options(infinite_nan PRIVATE "-Wno-strict-aliasing")
	target_compile_options(list_initialization PRIVATE "-Wno-uninitialized")
endif()
if(MSVC)
	target_compile_options(binary_indexed_tree PRIVATE "/wd4146")
endif()

# plugin
add_executable(plugin
	${source_dir}/design_pattern/plugin/plugin.cpp
	${source_dir}/design_pattern/plugin/B.hpp
)
target_link_libraries(plugin dl)
set_target_properties(plugin PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/design_pattern/plugin"
)
add_library(D SHARED
	${source_dir}/design_pattern/plugin/D.cpp
	${source_dir}/design_pattern/plugin/B.hpp
)
set_target_properties(D PROPERTIES
	PREFIX ""
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/design_pattern/plugin"
)

# PImpl
add_executable(pimpl
	${source_dir}/design_pattern/pimpl/pimpl.cpp
)
target_link_libraries(pimpl Widget)
set_target_properties(pimpl PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/design_pattern/pimpl"
)
add_library(Widget SHARED
	${source_dir}/design_pattern/pimpl/Widget.cpp
)
set_target_properties(Widget PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/design_pattern/pimpl"
)

# Google Test
find_package(GTest)
if(GTest_FOUND)
	add_executable(mock
		${source_dir}/google_test/mock.cpp
	)
	target_link_libraries(mock
		gtest
		gmock
		pthread
	)
	set_target_properties(mock PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/google_test"
	)
endif()

# test
if(EXISTS ${PROJECT_SOURCE_DIR}/test)
	add_subdirectory(test)
endif()
