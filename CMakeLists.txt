cmake_minimum_required(VERSION 3.7)
project(cpp_example LANGUAGES CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#---------------------------------------------------------------------------------------------------
# Compilation options.

set(CMAKE_CXX_STANDARD 17)
if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Ox /openmp -d2SSAOptimizer-") # -d2SSAOptimizer- for MSVC bug
else()
	set(CXX_WARNINGS "-Wall -Wextra -Wpointer-arith -Wcast-align -fstrict-aliasing")
	if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
		set(CXX_WARNINGS "${CXX_WARNINGS} -Wno-maybe-uninitialized")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wno-unused-but-set-variable")
	endif()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_WARNINGS} -fvisibility-inlines-hidden -march=native")

	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Wno-unused-variable")

	# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fopenmp")

	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
endif()

set(include_dir)
set(link_lib)

#---------------------------------------------------------------------------------------------------
# Third party libraries.

# googletest
add_subdirectory(thirdparty/googletest)
set_target_properties(gtest PROPERTIES
	FOLDER "thirdparty"
)
set_target_properties(gtest_main PROPERTIES
	FOLDER "thirdparty"
)
set_target_properties(gmock PROPERTIES
	FOLDER "thirdparty"
)
set_target_properties(gmock_main PROPERTIES
	FOLDER "thirdparty"
)

# benchmark
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE INTERNAL "")
set(BENCHMARK_ENABLE_TESTING OFF CACHE INTERNAL "")
add_subdirectory(thirdparty/benchmark)
set_target_properties(benchmark PROPERTIES
	FOLDER "thirdparty"
)
set_target_properties(benchmark_main PROPERTIES
	FOLDER "thirdparty"
)

list(APPEND include_dir
	thirdparty
	thirdparty/benchmark/include
	thirdparty/FP16/include
	thirdparty/half/include
)
list(APPEND link_lib
	gmock
	benchmark::benchmark
)

#---------------------------------------------------------------------------------------------------
# Source files.

set(source_dir example)
set(source_files
	algorithm/a_star.cpp
	algorithm/Dijkstra.cpp
	algorithm/Floyd.cpp
	algorithm/Hungarian.cpp
	algorithm/Kruskal.cpp
	algorithm/merge_sort.cpp
	algorithm/Prim.cpp
	algorithm/quick_sort.cpp
	algorithm/SPFA.cpp
	algorithm/topological_sort.cpp

	linear_algebra/armadillo/shared_memory.cpp
	linear_algebra/EOR1MP.cpp
	linear_algebra/matrix_multiplication.cpp

	autodiff/forward_with_dual.cpp # non_windows
	autodiff/Adept.cpp
	autodiff/Adept_valarray.cpp

	data_structure/binary_indexed_tree.cpp
	data_structure/disjoint_sets.cpp
	data_structure/priority_queue.cpp
	data_structure/mutable_priority_queue.cpp
	data_structure/segment_tree.cpp
	data_structure/sparse_table.cpp
	data_structure/trie.cpp

	feature/arithmetic.cpp # non_windows
	feature/class_size.cpp # non_windows
	feature/comma_operator.cpp
	feature/data_member_offset.cpp
	feature/float_half_conversion.cpp
	feature/infinite_nan.cpp
	feature/iterator.cpp
	feature/list_initialization.cpp
	feature/new_delete.cpp
	feature/virtual_base_class.cpp # non_windows
	feature/virtual_function.cpp # non_windows

	functional_programming/actor_with_reactive_stream/actor_with_reactive_stream.cpp
	functional_programming/actor_with_reactive_stream/actor_with_reactive_stream_test.cpp
	functional_programming/currying.cpp
	functional_programming/lazy_evaluation.cpp
	functional_programming/lazy_string_concatenation.cpp
	functional_programming/maybe_monad.cpp
	functional_programming/recursive_memoization.cpp
	functional_programming/ranges.cpp
	functional_programming/sum_type_for_state.cpp # non_windows

	google_test/mock.cpp # non_windows

	metaprogramming/binary.cpp
	metaprogramming/CRTP.cpp
	metaprogramming/dsl/dsl.cpp
	metaprogramming/has_type_.cpp
	metaprogramming/measurement_unit.cpp
	metaprogramming/nested_constructor.cpp
	metaprogramming/policy/policy.cpp
	metaprogramming/sizes_of.cpp
	metaprogramming/short_circuit.cpp
	metaprogramming/VarTypeDict.cpp

	simd/float_array_multiplication.cpp

	standard_library/array_compare.cpp
	standard_library/regex.cpp # non_windows
	standard_library/stringstream_eof.cpp # non_windows
	standard_library/valarray.cpp
	standard_library/wide_character.cpp # non_windows
)

if(MSVC)
set(source_files_non_windows
	autodiff/forward_with_dual.cpp
	feature/arithmetic.cpp
	feature/class_size.cpp
	feature/virtual_base_class.cpp
	feature/virtual_function.cpp
	functional_programming/sum_type_for_state.cpp
	google_test/mock.cpp
	standard_library/regex.cpp
	standard_library/stringstream_eof.cpp
	standard_library/wide_character.cpp
)
list(REMOVE_ITEM source_files ${source_files_non_windows})
endif()

#---------------------------------------------------------------------------------------------------
# Find packages.

# range-v3 required by
# 	functional_programming/actor_with_reactive_stream/actor_with_reactive_stream_test
# 	functional_programming/ranges
find_package(range-v3 QUIET)
if(NOT range-v3_FOUND)
	list(REMOVE_ITEM source_files
		functional_programming/actor_with_reactive_stream/actor_with_reactive_stream_test.cpp
		functional_programming/ranges.cpp
	)
	set(skip/functional_programming/actor_with_reactive_stream/actor_with_reactive_stream_test TRUE)
	set(skip/functional_programming/ranges TRUE)
endif()

# Boost_system required by
# 	functional_programming/actor_with_reactive_stream
find_package(Boost QUIET COMPONENTS system)
if(NOT Boost_SYSTEM_FOUND)
	list(REMOVE_ITEM source_files
		functional_programming/actor_with_reactive_stream/actor_with_reactive_stream.cpp
	)
	set(skip/functional_programming/actor_with_reactive_stream/actor_with_reactive_stream TRUE)
endif()

# Armadillo required by
# 	linear_algebra/armadillo/shared_memory
# 	linear_algebra/EOR1MP
find_package(Armadillo QUIET)
if(NOT ARMADILLO_FOUND)
	list(REMOVE_ITEM source_files
		linear_algebra/armadillo/shared_memory.cpp
		linear_algebra/EOR1MP.cpp
		linear_algebra/matrix_multiplication.cpp
	)
	set(skip/linear_algebra/armadillo/shared_memory TRUE)
	set(skip/linear_algebra/EOR1MP TRUE)
	set(skip/linear_algebra/matrix_multiplication TRUE)
else()
	list(APPEND include_dir ${ARMADILLO_INCLUDE_DIRS})
endif()

# Eigen3 required by
# 	linear_algebra/matrix_multiplication
find_package(Eigen3 QUIET)
if(NOT EIGEN3_FOUND)
	list(REMOVE_ITEM source_files
		linear_algebra/matrix_multiplication.cpp
	)
	set(skip/linear_algebra/matrix_multiplication TRUE)
else()
	list(APPEND include_dir ${EIGEN3_INCLUDE_DIR})
endif()

#---------------------------------------------------------------------------------------------------
# Build targets.

include_directories(${include_dir})
link_libraries(${link_lib})
foreach(src ${source_files})
	get_filename_component(bin ${src} NAME_WE)
	string(REGEX MATCH "^[a-z_]*/" dir ${src})
	add_executable(${bin}
		${source_dir}/${src}
	)
	get_filename_component(catagory ${src} DIRECTORY)
	set_target_properties(${bin} PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${dir}"
		FOLDER ${catagory}
	)
endforeach()
if(NOT MSVC)
	target_compile_options(infinite_nan PRIVATE "-Wno-strict-aliasing")
	target_compile_options(list_initialization PRIVATE "-Wno-uninitialized")
endif()
if(MSVC)
	target_compile_options(binary_indexed_tree PRIVATE "/wd4146")
endif()

if(NOT MSVC)
# feature/arithmetic warning
set_target_properties(arithmetic PROPERTIES
	COMPILE_FLAGS -Wno-sign-compare
)
# feature/class_size warning
set_target_properties(class_size PROPERTIES
	COMPILE_FLAGS -Wno-unused-private-field
)
# feature/data_member_offset warning
set_target_properties(data_member_offset PROPERTIES
	COMPILE_FLAGS -Wno-format
)
# feature/new_delete warning
set_target_properties(new_delete PROPERTIES
	COMPILE_FLAGS -Wno-unused-private-field
)
endif()

# functional_programming/actor_with_reactive_stream linking
if(NOT skip/functional_programming/actor_with_reactive_stream/actor_with_reactive_stream)
	target_link_libraries(actor_with_reactive_stream ${Boost_SYSTEM_LIBRARY})
endif()
# linear_algebra/EOR1MP linking
if(NOT skip/linear_algebra/EOR1MP)
	target_link_libraries(EOR1MP ${ARMADILLO_LIBRARY})
endif()
# linear_algebra/matrix_multiplication linking
if(NOT skip/linear_algebra/matrix_multiplication)
	target_link_libraries(matrix_multiplication ${ARMADILLO_LIBRARY})
endif()

# design_pattern/plugin
add_executable(plugin
	${source_dir}/design_pattern/plugin/plugin.cpp
	${source_dir}/design_pattern/plugin/B.hpp
)
if(NOT MSVC)
	target_link_libraries(plugin dl)
endif()
set_target_properties(plugin PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/design_pattern/plugin"
	FOLDER "design_pattern/plugin"
)
add_library(D SHARED
	${source_dir}/design_pattern/plugin/D.cpp
	${source_dir}/design_pattern/plugin/B.hpp
)
if(NOT MSVC)
	set_target_properties(D PROPERTIES
		PREFIX ""
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/design_pattern/plugin"
		FOLDER "design_pattern/plugin"
	)
else()
	set_target_properties(D PROPERTIES
		PREFIX ""
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/design_pattern/plugin"
		FOLDER "design_pattern/plugin"
	)
endif()

# design_pattern/PImpl
add_executable(PImpl
	${source_dir}/design_pattern/PImpl/PImpl.cpp
)
target_link_libraries(PImpl Widget)
set_target_properties(PImpl PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/design_pattern/PImpl"
	FOLDER "design_pattern/PImpl"
)
if(NOT MSVC)
	add_library(Widget SHARED
		${source_dir}/design_pattern/PImpl/Widget.cpp
	)
	set_target_properties(Widget PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/design_pattern/PImpl"
		FOLDER "design_pattern/PImpl"
	)
else()
	add_library(Widget STATIC
		${source_dir}/design_pattern/PImpl/Widget.cpp
	)
	set_target_properties(Widget PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/design_pattern/PImpl"
		FOLDER "design_pattern/PImpl"
	)
endif()

# test
if(EXISTS ${PROJECT_SOURCE_DIR}/test)
	add_subdirectory(test)
endif()
