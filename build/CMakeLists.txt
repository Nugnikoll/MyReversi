cmake_minimum_required (VERSION 2.6)
project (reversi)
set (REVERSI_VERSION 2.1)

set (CMAKE_VERBOSE_MAKEFILE ON)

include (CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG ("-std=c++11" COMPILER_SUPPORTS_CXX11)
if (NOT COMPILER_SUPPORTS_CXX11)
    message(FATAL_ERROR "Compiler ${CMAKE_CXX_COMPILER} has no C++11 support.")
endif ()

set (cpp_dir "${PROJECT_SOURCE_DIR}/../cpp")
set (python_dir "${PROJECT_SOURCE_DIR}/../python")
set (cpuid_dir "${PROJECT_SOURCE_DIR}/../cpuid")

if (WIN32)
	if ("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -D_hypot=hypot -DMS_WIN64 -DUSE_CMAKE")
	else ()
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -D_hypot=hypot -DMS_WIN32 -DUSE_CMAKE")
	endif ()
else ()
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -DUSE_CMAKE")
endif ()
set (CMAKE_CXX_FLAGS_DEBUG "-Og -g -DDEBUG")
set (CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

try_run (
	CPUID_RUN_RESULT CPUID_COMPILE_RESULT
	${cpuid_dir} ${cpuid_dir}/cpuid.cc
	RUN_OUTPUT_VARIABLE CPUID_OUTPUT_VARIABLE
)

if ("${CPUID_COMPILE_RESULT}" STREQUAL "TRUE")
	message ("-- Successfully compile a program with inline assembly.")
	if (NOT ("${CPUID_RUN_RESULT}" STREQUAL "FAILED_TO_RUN"))
		message ("${CPUID_OUTPUT_VARIABLE}")
		message ("-- Successfully execute a program with inline assembly. Define macro USE_ASM.")
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_ASM")
		string (FIND ${CPUID_OUTPUT_VARIABLE} "bmi2 " POS_BMI2)
		if (NOT ("${POS_BMI2}" EQUAL "-1"))
			message ("-- BMI2 instruction set is supported. Define macro USE_ASM_BMI2.")
			set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_ASM_BMI2")
		endif ()
		string (FIND ${CPUID_OUTPUT_VARIABLE} "avx2 " POS_AVX2)
		if (NOT ("${POS_AVX2}" EQUAL "-1"))
			message ("-- AVX2 instruction set is supported. Define macro USE_ASM_AVX2.")
			set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_ASM_AVX2")
		endif ()
	else ()
		message ("-- Failed to execute a program with inline assembly..")
	endif ()
else ()
	message ("-- Failed to complile a program with inline assembly.")
endif ()

set (CMAKE_SHARED_LIBRARY_PREFIX "")

include (FindPythonLibs)
include (FindPythonInterp)
message ("-- Locate python libraries at ${PYTHON_LIBRARIES}")
execute_process(
	COMMAND "${PYTHON_EXECUTABLE}" "-c"
	"import numpy; print(numpy.get_include());"
	OUTPUT_VARIABLE NUMPY_INCLUDE_DIR
)
message ("-- Locate NumPy include files at ${NUMPY_INCLUDE_DIR}")

include_directories ("${PYTHON_INCLUDE_DIRS}" "${NUMPY_INCLUDE_DIR}")

set (
	head_file
	${cpp_dir}/type.h
	${cpp_dir}/asm.h
	${cpp_dir}/board.h
	${cpp_dir}/hash.h
	${cpp_dir}/log.h
	${cpp_dir}/pattern.h
	${cpp_dir}/tree.h
)

set (
	source_file
	${cpp_dir}/board.cc
	${cpp_dir}/flip.cc
	${cpp_dir}/pattern.cc
	${cpp_dir}/search.cc
	${python_dir}/reversi_wrap.cxx
)

add_custom_command (
	OUTPUT ${python_dir}/reversi_wrap.cxx
	COMMAND swig -python -c++ -threads -w-314,-362,-389,-503,-509 -o ${python_dir}/reversi_wrap.cxx ${python_dir}/reversi.i
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
