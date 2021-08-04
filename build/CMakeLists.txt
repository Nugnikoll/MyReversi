cmake_minimum_required (VERSION 3.14)
project (reversi)
set (REVERSI_VERSION 2.3)

set (CMAKE_VERBOSE_MAKEFILE ON)

include (CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG ("-std=c++17" COMPILER_SUPPORTS_CXX17)
if (NOT COMPILER_SUPPORTS_CXX17)
    message(FATAL_ERROR "Compiler ${CMAKE_CXX_COMPILER} has no C++17 support.")
endif ()

find_package (Python3 REQUIRED COMPONENTS NumPy Interpreter Development)
find_package (SWIG 3.0.6 REQUIRED)

set (cpp_dir "${PROJECT_SOURCE_DIR}/../cpp")
set (python_dir "${PROJECT_SOURCE_DIR}/../python")
set (cpuid_dir "${PROJECT_SOURCE_DIR}/../cpuid")
set (bot_dir "${PROJECT_SOURCE_DIR}/../bot")

set (
	head_file
	${cpp_dir}/type.h
	${cpp_dir}/asm.h
	${cpp_dir}/board.h
	${cpp_dir}/hash.h
	${cpp_dir}/log.h
	${cpp_dir}/pattern.h
	${cpp_dir}/tree.h
	${cpp_dir}/sample.h
)

set (
	source_file
	${cpp_dir}/jsoncpp.cc
	${cpp_dir}/board.cc
	${cpp_dir}/flip.cc
	${cpp_dir}/pattern.cc
	${cpp_dir}/search.cc
	${cpp_dir}/sample.cc
)

set (
	wrap_file
	${python_dir}/reversi_wrap.cc
)

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

if (NOT "${CMAKE_FORCE_ASM}" STREQUAL "")
	if ("${CMAKE_FORCE_ASM}" STREQUAL "asm")
		message ("-- Define macro USE_ASM.")
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_ASM")
	elseif ("${CMAKE_FORCE_ASM}" STREQUAL "avx2")
		message ("-- Define macro USE_ASM.")
		message ("-- Define macro USE_ASM_BMI2.")
		message ("-- Define macro USE_ASM_AVX2.")
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_ASM -DUSE_ASM_BMI2 -DUSE_ASM_AVX2")
	else ()
		message (FATAL_ERROR "-- Unexpected macro value.")
	endif ()
	add_executable (cpuid ${cpuid_dir}/cpuid.cc)
	add_executable (bot ${bot_dir}/sample/main.cc ${source_file} ${head_file})
	set_property(TARGET bot PROPERTY CXX_STANDARD 17)
	set_property(TARGET bot PROPERTY CXX_STANDARD_REQUIRED ON)
	target_compile_options(bot PRIVATE "-DUSE_TERMINATE")
	get_target_property(BOT_LINK_FLAGS bot LINK_FLAGS)
	if("${BOT_LINK_FLAGS}" STREQUAL "BOT_LINK_FLAGS-NOTFOUND")
		set (BOT_LINK_FLAGS "")
	endif ()
	set (BOT_LINK_FLAGS "${BOT_LINK_FLAGS} -static")
	if (NOT WIN32)
		set(BOT_LINK_FLAGS "${BOT_LINK_FLAGS} -pthread -Wl,--whole-archive -lpthread -Wl,--no-whole-archive")
	endif ()
	set_target_properties(bot PROPERTIES LINK_FLAGS ${BOT_LINK_FLAGS})
else ()
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
endif ()

set (CMAKE_SHARED_LIBRARY_PREFIX "")

include_directories ("${Python3_INCLUDE_DIRS}" "${Python3_NumPy_INCLUDE_DIRS}")

if ("${SWIG_VERSION}" VERSION_GREATER_EQUAL "4.0.0")
	set (CMAKE_SWIG_EXTRA_FLAGS "-doxygen")
else ()
	set (CMAKE_SWIG_EXTRA_FLAGS "")
endif ()

add_custom_command (
	OUTPUT ${python_dir}/reversi_wrap.cc
	COMMAND ${SWIG_EXECUTABLE} -python -c++ -threads -w-314,-362,-389,-503,-509 ${CMAKE_SWIG_EXTRA_FLAGS} -o ${python_dir}/reversi_wrap.cc ${python_dir}/reversi.i
	DEPENDS ${python_dir}/reversi.i ${head_file}
)

add_library(_reversi SHARED ${source_file} ${wrap_file})
set_property(TARGET _reversi PROPERTY CXX_STANDARD 17)
set_property(TARGET _reversi PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET _reversi PROPERTY CXX_EXTENSIONS OFF)
target_link_libraries (_reversi ${Python3_LIBRARIES})

if ("${CMAKE_FORCE_ASM}" STREQUAL "")
	if (WIN32)
		add_custom_command(TARGET _reversi POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:_reversi> ${python_dir}/_reversi.pyd
		)
	else ()
		add_custom_command(TARGET _reversi POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:_reversi> ${python_dir}/_reversi.so
		)
	endif ()
endif()
