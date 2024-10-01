# 3.13 is required for:
# - relative source file paths
# - NEW behavior of CMP0077 (consistent behavior between build runs when a variables or cache entry is identical to an option).
#   This effectively makes it possible to set option-named-variables before first build run.
cmake_minimum_required(VERSION 3.13)

project(nukleus
	VERSION 1.0.0 # how to do versioning? base on nuklear?
	DESCRIPTION "modern C++ wrapper library for Nuklear C library"
	LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message(STATUS "No CMAKE_BUILD_TYPE specified - no optimization and debug flags will be applied")
endif()

##############################################################################
# options for customizing the build

option(NK_PRIVATE "TODO ???" OFF)

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
option(NK_BUTTON_TRIGGER_ON_RELEASE "ON: react on button release. OFF: react on button press." OFF)
option(NK_KEYSTATE_BASED_INPUT "ON: use key state for each frame. OFF: use key press/release events." OFF)
option(NK_ZERO_COMMAND_MEMORY "ON: zero out memory for each drawing command added to a drawing queue (inside nk_command_buffer_push). Zeroing command memory is very useful for fast checking (using memcmp) if command buffers are equal and avoid drawing frames when nothing on screen has changed since previous frame." ON)

##############################################################################
# Nukleus-specific options

option(NUKLEUS_BUILD_SEPARATELY
"ON: Build Nukleus as a separate target. Limited build configuration control through CMake options.\
OFF: Follow Nuklear's header and implementation modes. NK_* CMake options have no effect.\
You must include your own defines before including this library. One inclusion has to have NK_IMPLEMENTATION defined.\
You can also drop using CMake entirely and use this library exactly the same as if it was just Nuklear."
OFF)
option(NUKLEUS_BUILD_SHARED_LIB "ON: Build Nukleus as a shared library object. OFF: as static. This option has no effect if NUKLEUS_BUILD_SEPARATELY is OFF" OFF)
option(NUKLEUS_USE_LTO "ON: use CMake's built-in LTO support and apply it to Nukleus targets. This option has no effect if NUKLEUS_BUILD_SEPARATELY is OFF" OFF)

##############################################################################
# LTO support

if(NUKLEUS_USE_LTO)
	include(CheckIPOSupported)
	check_ipo_supported(RESULT IPO_SUPPORTED OUTPUT IPO_ERROR)

	if(IPO_SUPPORTED)
		message(STATUS "Nukleus: IPO / LTO supported, enabling")
	else()
		message(FATAL_ERROR "Nukleus: IPO / LTO not supported: ${IPO_ERROR}, change settings or compiler")
	endif()
endif()

##############################################################################

if(NOT NUKLEAR_HEADER_DIR)
	message(FATAL "Specify path to directory with nuklear.h by setting NUKLEAR_HEADER_DIR")
endif()

set(NUKLEUS_SOURCES
	src/nukleus/nukleus.hpp
)

if(NUKLEUS_BUILD_SEPARATELY)
	if(NUKLEUS_BUILD_SHARED_LIB)
		add_library(nukleus SHARED)
	else()
		add_library(nukleus STATIC)
	endif()

	target_sources(nukleus PUBLIC ${NUKLEUS_SOURCES} src/nukleus/nukleus.cpp)
	target_include_directories(nukleus PUBLIC ${NUKLEAR_HEADER_DIR} src)
	target_compile_options(nukleus PUBLIC
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

	include(CMakePrintHelpers)
	function(option_to_define OPTION_NAME)
		if(${OPTION_NAME})
			target_compile_definitions(nukleus PUBLIC ${OPTION_NAME})
		endif()
		cmake_print_variables(${OPTION_NAME})
	endfunction()

	option_to_define(NK_PRIVATE)

	option_to_define(NK_INCLUDE_FIXED_TYPES)
	option_to_define(NK_INCLUDE_STANDARD_BOOL)

	option_to_define(NK_INCLUDE_DEFAULT_ALLOCATOR)
	option_to_define(NK_INCLUDE_STANDARD_IO)
	option_to_define(NK_INCLUDE_STANDARD_VARARGS)

	option_to_define(NK_INCLUDE_FONT_BAKING)
	option_to_define(NK_INCLUDE_DEFAULT_FONT)

	option_to_define(NK_INCLUDE_VERTEX_BUFFER_OUTPUT)
	option_to_define(NK_INCLUDE_COMMAND_USERDATA)
	option_to_define(NK_UINT_DRAW_INDEX)

	option_to_define(NK_BUTTON_TRIGGER_ON_RELEASE)
	option_to_define(NK_KEYSTATE_BASED_INPUT)
	option_to_define(NK_ZERO_COMMAND_MEMORY)
else()
	add_library(nukleus INTERFACE)
	target_sources(nukleus INTERFACE ${NUKLEUS_SOURCES})
	target_include_directories(nukleus INTERFACE ${NUKLEAR_HEADER_DIR} src)
endif()
