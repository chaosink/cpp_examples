cmake_minimum_required(VERSION 3.7)
project(cpp_example LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
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
	iterator.cpp
	comma_operator.cpp
)

foreach(src ${source_files})
	get_filename_component(bin ${src} NAME_WE)
	add_executable(${bin}
		"${source_dir}/${common_head}"
		"${source_dir}/${src}"
	)
endforeach()
