cmake_minimum_required(VERSION 3.7)
project(cpp_example LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /openmp")
else()
	set(CXX_WARNINGS "-Wall -Wextra -Wpointer-arith -Wcast-align -fstrict-aliasing \
		-Wno-unused-local-typedefs -Wno-implicit-fallthrough")
	if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
		set(CXX_WARNINGS "${CXX_WARNINGS} -Wno-uninitialized")
	else()
		set(CXX_WARNINGS "${CXX_WARNINGS} -Wno-maybe-uninitialized \
			-Wno-misleading-indentation -Wno-int-in-bool-context ")
	endif()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_WARNINGS} -fvisibility-inlines-hidden \
		-fopenmp")
endif()

set(source_dir examples)
set(common_head common.hpp)
set(source_files

	features/comma_operator.cpp
	features/infinite_nan.cpp
	features/iterator.cpp

	functional_programming/maybe_monad.cpp

	metaprogramming/binary.cpp
	metaprogramming/has_type_.cpp
	metaprogramming/sizes_of.cpp
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
