# 3.13 is required for:
# - relative source file paths
# - NEW behavior of CMP0077 (consistent behavior between build runs when a variables or cache entry is identical to an option).
#   This effectively makes it possible to set option-named-variables before first build run.
cmake_minimum_required(VERSION 3.13)

project(nukleus
	DESCRIPTION "modern C++ wrapper library for Nuklear C library"
	LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message(STATUS "No CMAKE_BUILD_TYPE specified - no optimization and debug flags will be applied")
endif()

include(CMakePrintHelpers)

##############################################################################
# Options for customizing the build.
# These only affect the xev::nukleus target.
# Refer to Nuklear's documentation for more details.

option(NK_PRIVATE
"ON: declare all Nuklear's functions as static (equivalent to anonymous namespace in C++).\
This option reduces amount of exposed linker symbols but prevents their use in other translation units."
OFF)

# stdlib options - only implementation (no changes in the API or behavior)
option(NK_INCLUDE_FIXED_TYPES "ON: include <stdint.h> for fixed-width types. OFF: use Nuklear's own implementation." ON)
option(NK_INCLUDE_STANDARD_BOOL "ON: include <stdbool.h>. OFF: use Nuklear's typedef int nk_bool." ON)

# stdlib options - API additions
option(NK_INCLUDE_DEFAULT_ALLOCATOR "ON: include <stdlib.h> and provide additional functions to use this library without caring for memory allocation control." ON)
option(NK_INCLUDE_STANDARD_IO "ON: include <stdio.h> and provide additional functions dependent on filesystem API." ON)
option(NK_INCLUDE_STANDARD_VARARGS "ON: include <stdarg.h> and provide additional functions utilizing C variadic arguments." ON)

# font options
option(NK_INCLUDE_FONT_BAKING "ON: add stb_truetype and stb_rect_pack implementation and provide font baking and rendering. If you already have font handling or do not want to use this font handler you don't have to define it." ON)
option(NK_INCLUDE_DEFAULT_FONT "ON: add ProggyClean.ttf into this library which can be loaded into a font atlas and allow using this library without having a truetype font." ON)

# rendering options
option(NK_INCLUDE_VERTEX_BUFFER_OUTPUT "ON: add vertex draw command list backend to allow to convert queue commands into vertex draw commands (mainly if you need a hardware accessible format for OpenGL, DirectX, Vulkan, Metal, ...)." ON)
option(NK_INCLUDE_COMMAND_USERDATA "ON: add a userdata pointer into each command. Can be useful for example if you want to provide custom shaders depending on the used widget. Can be combined with the style structures." ON)
option(NK_UINT_DRAW_INDEX "ON: use 32-bit vertex index elements. OFF: 16-bit. This option has no effect if NK_VERTEX_BUFFER_OUTPUT is OFF" ON)

# behavior options
option(NK_BUTTON_TRIGGER_ON_RELEASE "ON: react on button release. OFF: react on button press." ON)
option(NK_KEYSTATE_BASED_INPUT "ON: use key state for each frame. OFF: use key press/release events." OFF)
option(NK_ZERO_COMMAND_MEMORY "ON: zero out memory for each drawing command added to a drawing queue (inside nk_command_buffer_push). Zeroing command memory is very useful for fast checking (using memcmp) if command buffers are equal and avoid drawing frames when nothing on screen has changed since previous frame." ON)

##############################################################################
# Nukleus-specific options

option(NUKLEUS_USE_CPP14 "ON: use C++14 standard when compiling" OFF)
option(NUKLEUS_USE_CPP17 "ON: use C++17 standard when compiling" OFF)
option(NUKLEUS_USE_CPP20 "ON: use C++20 standard when compiling" OFF)
option(NUKLEUS_USE_CPP23 "ON: use C++23 standard when compiling" OFF)

option(NUKLEUS_USE_CHARCONV "ON: use <charconv> when in C++17 or higher and when NK_DTOA and NUKLEUS_AVOID_STDLIB are not defined" OFF)

option(NUKLEUS_BUILD_DEMO "ON: Build Nukleus sample application. Requires SDL >= 2.0.18." ON)
option(NUKLEUS_BUILD_SHARED_LIB "ON: Build xev::nukleus target as a shared library object. OFF: as static." OFF)
option(NUKLEUS_USE_LTO "ON: use CMake's built-in LTO support and apply it to xev::nukleus target" OFF)
option(NUKLEUS_ENABLE_SANITIZERS "build with -fsanitize=address -fsanitize=undefined" OFF)

##############################################################################
# LTO (IPO) support

if(NUKLEUS_USE_LTO)
	include(CheckIPOSupported)
	check_ipo_supported(RESULT IPO_SUPPORTED OUTPUT IPO_ERROR)

	if(IPO_SUPPORTED)
		message(STATUS "Nukleus: IPO / LTO supported, enabling")
	else()
		message(FATAL_ERROR "Nukleus: IPO / LTO not supported: ${IPO_ERROR}, disable NUKLEUS_USE_LTO or change compiler")
	endif()
endif()

##############################################################################
# Common stuff for all targets

if(NOT NUKLEAR_HEADER_DIR)
	message(FATAL_ERROR "Specify path to directory with nuklear.h by setting NUKLEAR_HEADER_DIR")
endif()

function(apply_nukleus_warning_flags TARGET)
	target_compile_options(${TARGET} PRIVATE
		# GCC's -Wconvertion is too aggressive with no way to silence it
		$<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic -Wno-deprecated-copy -ffast-math
			-Wsuggest-attribute=noreturn -Wduplicated-branches -Wduplicated-cond -Wfloat-equal
			-Wsign-conversion -Wlogical-op -Wmissing-declarations
			-Wmismatched-tags -Wextra-semi -Wsuggest-override>
		$<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic -Wdeprecated -Wdocumentation -ffast-math
			-Wabstract-vbase-init -Wanon-enum-enum-conversion -Warray-bounds-pointer-arithmetic
			-Wconversion -Wbad-function-cast -Wcast-qual -Wzero-as-null-pointer-constant
			-Wconditional-uninitialized -Wextra-semi -Wextra-semi-stmt -Wfloat-equal
			-Widiomatic-parentheses -Wimplicit-fallthrough -Wmissing-noreturn -Wmissing-variable-declarations
			-Wnewline-eof -Wnon-virtual-dtor -Wsuggest-override -Wundefined-reinterpret-cast
			-Wunneeded-internal-declaration -Wunreachable-code-aggressive -Wunsupported-dll-base-class-template>
		# not tried, likely many disables missing
		$<$<CXX_COMPILER_ID:MSVC>:/W4>
	)
endfunction()

function(apply_nukleus_cxx_std TARGET)
	if (NUKLEUS_USE_CPP23)
		target_compile_features(${TARGET} INTERFACE cxx_std_23)
	elseif (NUKLEUS_USE_CPP20)
		target_compile_features(${TARGET} INTERFACE cxx_std_20)
	elseif (NUKLEUS_USE_CPP17)
		target_compile_features(${TARGET} INTERFACE cxx_std_17)
	elseif (NUKLEUS_USE_CPP14)
		target_compile_features(${TARGET} INTERFACE cxx_std_14)
	else()
		target_compile_features(${TARGET} INTERFACE cxx_std_11)
	endif()
endfunction()

##############################################################################
# Interface target

add_library(nukleus_headers INTERFACE)
target_sources(nukleus_headers INTERFACE nukleus.hpp ${NUKLEAR_HEADER_DIR}/nuklear.h)
target_include_directories(nukleus_headers INTERFACE . ${NUKLEAR_HEADER_DIR})
apply_nukleus_cxx_std(nukleus_headers)

if(NUKLEUS_ENABLE_SANITIZERS)
	target_compile_options(nukleus_headers INTERFACE -fsanitize=address -fsanitize=undefined)
	target_link_options(nukleus_headers INTERFACE -fsanitize=address -fsanitize=undefined)
endif()

add_library(xev::nukleus_headers ALIAS nukleus_headers)

##############################################################################
# Static/dynamic library target

if(NUKLEUS_BUILD_SHARED_LIB)
	add_library(nukleus SHARED)
else()
	add_library(nukleus STATIC)
endif()

target_sources(nukleus
	PUBLIC nukleus.hpp ${NUKLEAR_HEADER_DIR}/nuklear.h
	PRIVATE nk_implementation.cpp)
target_include_directories(nukleus PUBLIC . ${NUKLEAR_HEADER_DIR})

apply_nukleus_warning_flags(nukleus)

function(cmake_option_to_compiler_define OPTION_NAME)
	if(${OPTION_NAME})
		target_compile_definitions(nukleus PUBLIC ${OPTION_NAME})
	endif()
	cmake_print_variables(${OPTION_NAME})
endfunction()

cmake_option_to_compiler_define(NK_PRIVATE)

cmake_option_to_compiler_define(NK_INCLUDE_FIXED_TYPES)
cmake_option_to_compiler_define(NK_INCLUDE_STANDARD_BOOL)

cmake_option_to_compiler_define(NK_INCLUDE_DEFAULT_ALLOCATOR)
cmake_option_to_compiler_define(NK_INCLUDE_STANDARD_IO)
cmake_option_to_compiler_define(NK_INCLUDE_STANDARD_VARARGS)

cmake_option_to_compiler_define(NK_INCLUDE_FONT_BAKING)
cmake_option_to_compiler_define(NK_INCLUDE_DEFAULT_FONT)

cmake_option_to_compiler_define(NK_INCLUDE_VERTEX_BUFFER_OUTPUT)
cmake_option_to_compiler_define(NK_INCLUDE_COMMAND_USERDATA)
cmake_option_to_compiler_define(NK_UINT_DRAW_INDEX)

cmake_option_to_compiler_define(NK_BUTTON_TRIGGER_ON_RELEASE)
cmake_option_to_compiler_define(NK_KEYSTATE_BASED_INPUT)
cmake_option_to_compiler_define(NK_ZERO_COMMAND_MEMORY)

apply_nukleus_cxx_std(nukleus)

if(NUKLEUS_ENABLE_SANITIZERS)
	target_compile_options(nukleus PUBLIC -fsanitize=address -fsanitize=undefined)
	target_link_options(nukleus PUBLIC -fsanitize=address -fsanitize=undefined)
endif()

add_library(xev::nukleus ALIAS nukleus)

##############################################################################
# Demo applications

if(NUKLEUS_BUILD_DEMO_SDL2) # later: use OR for any additional demos
	add_library(nukleus_demo_common STATIC)
	target_sources(nukleus_demo_common PRIVATE
		demo/common/canvas.cpp
		demo/common/common.hpp
		demo/common/node_editor.cpp
		demo/common/overview.cpp
		demo/common/style_configurator.cpp
		demo/common/style.cpp
	)
	apply_nukleus_cxx_std(nukleus_demo_common)
	apply_nukleus_warning_flags(nukleus_demo_common)
	target_compile_definitions(nukleus_demo_common PUBLIC
		NK_INCLUDE_FIXED_TYPES
		NK_INCLUDE_STANDARD_IO
		NK_INCLUDE_STANDARD_VARARGS
		NK_INCLUDE_DEFAULT_ALLOCATOR
		NK_INCLUDE_VERTEX_BUFFER_OUTPUT
		NK_INCLUDE_FONT_BAKING
		NK_INCLUDE_DEFAULT_FONT
	)
	target_include_directories(nukleus_demo_common PUBLIC demo)
	target_link_libraries(nukleus_demo_common PUBLIC xev::nukleus_headers)
endif()

if(NUKLEUS_BUILD_DEMO_SDL2)
	find_package(SDL2 REQUIRED)
	find_package(OpenGL REQUIRED)

	add_executable(nukleus_demo)
	target_sources(nukleus_demo PRIVATE demo/main_sdl2.cpp)
	apply_nukleus_cxx_std(nukleus_demo)
	apply_nukleus_warning_flags(nukleus_demo)

	# https://wiki.libsdl.org/SDL2/README-cmake
	# SDL2::SDL2main may or may not be available. It is e.g. required by Windows GUI applications
	if(TARGET SDL2::SDL2main)
		# It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or SDL2::SDL2-static)
		target_link_libraries(nukleus_demo PRIVATE SDL2::SDL2main)
	endif()

	target_link_libraries(nukleus_demo PRIVATE nukleus_demo_common SDL2::SDL2 OpenGL::GL)

	if (WIN32)
		# copy the .dll file to the same folder as the executable
		add_custom_command(
			TARGET nukleus_demo POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:SDL2::SDL2> $<TARGET_FILE_DIR:nukleus_demo>
			VERBATIM)
	endif()
endif()

##############################################################################
# Logs (all here to avoid mixing order with NK_* option logs)
cmake_print_variables(NUKLEAR_REPO_DIR)
cmake_print_variables(NUKLEAR_HEADER_DIR)
cmake_print_variables(NUKLEUS_BUILD_SHARED_LIB)
cmake_print_variables(NUKLEUS_BUILD_DEMO)
