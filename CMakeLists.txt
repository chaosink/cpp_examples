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

set(source_dir examples)
set(common_head common.hpp)
set(source_files
	algorithms/binary_indexed_tree.cpp
	algorithms/priority_queue.cpp
	algorithms/quick_sort.cpp
	algorithms/trie.cpp

	features/comma_operator.cpp
	features/infinite_nan.cpp
	features/iterator.cpp
	features/list_initialization.cpp

	functional_programming/maybe_monad.cpp

	metaprogramming/binary.cpp
	metaprogramming/has_type_.cpp
	metaprogramming/sizes_of.cpp
	metaprogramming/short-circuit.cpp
	metaprogramming/VarTypeDict.cpp
)

include_directories(${source_dir})
foreach(src ${source_files})
	get_filename_component(bin ${src} NAME_WE)
	add_executable(${bin}
		${source_dir}/${common_head}
		${source_dir}/${src}
	)
endforeach()
target_compile_options(infinite_nan PRIVATE "-Wno-strict-aliasing")

# plugin architecture
add_executable(plugin
	${source_dir}/plugin/plugin.cpp
	${source_dir}/plugin/B.hpp
)
target_link_libraries(plugin dl)
add_library(D SHARED
	${source_dir}/plugin/D.cpp
	${source_dir}/plugin/B.hpp
)
set_target_properties(D PROPERTIES PREFIX "")

# Google Test
add_executable(mock
	${source_dir}/google_test/mock.cpp
)
target_link_libraries(mock
	gtest
	gmock
	pthread
)
