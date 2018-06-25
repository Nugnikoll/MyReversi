cmake_minimum_required (VERSION 2.6)
project (reversi)
set (REVERSI_VERSION 2.1)

include (CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG ("-std=c++11" COMPILER_SUPPORTS_CXX11)
if (NOT COMPILER_SUPPORTS_CXX11)
    message(FATAL_ERROR "Compiler ${CMAKE_CXX_COMPILER} has no C++11 support.")
endif ()

include (FindPythonLibs)

set (cpp_dir "${PROJECT_SOURCE_DIR}/../cpp")
set (python_dir "${PROJECT_SOURCE_DIR}/../python")
if (WIN32)
	if ("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
		set (CMAKE_CXX_FLAGS "-Wall -D_hypot=hypot -DMS_WIN64 -Og -g")
	else ()
		set (CMAKE_CXX_FLAGS "-Wall -D_hypot=hypot -DMS_WIN32 -Og -g")
	endif ()
else ()
	set (CMAKE_CXX_FLAGS "-Wall -Og -g")
endif ()
#set (CMAKE_CXX_FLAGS_DEBUG "-Og -g -DEBUG")
#set (CMAKE_CXX_FLAGS_RELEASE "-O3 -NDEBUG")
set (CMAKE_SHARED_LIBRARY_PREFIX "")

include_directories ("${PYTHON_INCLUDE_DIRS}")

set (
	head_file
	${cpp_dir}/type.h
	${cpp_dir}/asm.h
	${cpp_dir}/reversi.h
	${cpp_dir}/hash.h
	${cpp_dir}/log.h
	${cpp_dir}/matrix.h
	${cpp_dir}/pattern.h
)

set (
	source_file
	${cpp_dir}/reversi.cc
	${cpp_dir}/flip.cc
	${cpp_dir}/pattern.cc
	${cpp_dir}/search.cc
	${python_dir}/reversi_wrap.cxx
)

add_custom_command (
	OUTPUT ${python_dir}/reversi_wrap.cxx
	COMMAND swig -python -c++ -threads -o ${python_dir}/reversi_wrap.cxx ${python_dir}/reversi.i
	DEPENDS ${python_dir}/reversi.i ${head_file}
)

add_library(_reversi SHARED ${source_file})
set_property(TARGET _reversi PROPERTY CXX_STANDARD 11)
set_property(TARGET _reversi PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET _reversi PROPERTY CXX_EXTENSIONS OFF)
target_link_libraries (_reversi ${PYTHON_LIBRARIES})

if (WIN32)
	add_custom_command(TARGET _reversi POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:_reversi> ${python_dir}/_reversi.pyd
	)
else ()
	add_custom_command(TARGET _reversi POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:_reversi> ${python_dir}/
	)
endif ()
