cmake_minimum_required(VERSION 3.10)

project(cmc-chess)

if(NOT CMAKE_BUILD_TYPE)
	message("Setting BUILD_TYPE to Release")
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

string(TOLOWER ${CMAKE_BUILD_TYPE} build_type)

set(CMAKE_C_FLAGS_RELEASE "")
set(CMAKE_C_FLAGS_DEBUG "")

set(SRC main.c piece.c board.c)
set(H "")

add_executable(cmc-chess ${SRC})

# This project is meant to be fun!
# The C standard is C89, strict ANSI.
# set_property(TARGET cmc-chess PROPERTY C_STANDARD 90)
# set_property(TARGET cmc-chess PROPERTY C_EXTENSIONS OFF)
target_compile_options(cmc-chess PRIVATE -std=c89)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	message("Compiler is supported.")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	message("Compiler is supported.")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	message(FATAL_ERROR "Your compiler is not supported.")
endif()

target_compile_options(cmc-chess PRIVATE 
	-pedantic -pedantic-errors -Werror
	-fanalyzer -fno-common -fPIC -Wfatal-errors

	# Style
	# -Wdeclaration-after-statement -Werror=declaration-after-statement
)

# Basic Warnings
target_compile_options(cmc-chess PRIVATE 
	-Wall
	-Wextra
	-Wabsolute-value
	-Waddress
	-Waddress-of-packed-member
	-Waggregate-return
	-Waggressive-loop-optimizations
	-Walloc-size-larger-than=1048576
	-Walloc-zero
	-Walloca
	-Walloca-larger-than=2048
	-Warith-conversion
	-Warray-bounds=2
	-Warray-compare
	-Wshadow
	-Wpointer-arith
	-Wcast-align
	-Wstrict-prototypes
	-Wmissing-prototypes
	-Wold-style-definition
	-Wformat=2
	-Wformat-truncation
	-Wstringop-overflow
	-Wundef
	-Wswitch-enum
	-Wbad-function-cast
	-Wconversion
	-Wsign-conversion
	-Wdouble-promotion
)

# Analyzer
target_compile_options(cmc-chess PRIVATE 
	-Wanalyzer-allocation-size
	-Wanalyzer-deref-before-check
	-Wanalyzer-double-fclose
	-Wanalyzer-double-free
	-Wanalyzer-exposure-through-output-file
	-Wanalyzer-exposure-through-uninit-copy
	-Wanalyzer-fd-access-mode-mismatch
	-Wanalyzer-fd-double-close
	-Wanalyzer-fd-leak
	-Wanalyzer-fd-phase-mismatch
	-Wanalyzer-fd-type-mismatch
	-Wanalyzer-fd-use-after-close
	-Wanalyzer-fd-use-without-check
	-Wanalyzer-file-leak
	-Wanalyzer-free-of-non-heap
	-Wanalyzer-imprecise-fp-arithmetic
	-Wanalyzer-infinite-recursion
	-Wanalyzer-jump-through-null
	-Wanalyzer-malloc-leak
	-Wanalyzer-mismatching-deallocation
	-Wanalyzer-null-argument
	-Wanalyzer-null-dereference
	-Wanalyzer-out-of-bounds
	-Wanalyzer-possible-null-argument
	-Wanalyzer-possible-null-dereference
	-Wanalyzer-putenv-of-auto-var
	-Wanalyzer-shift-count-negative
	-Wanalyzer-shift-count-overflow
	-Wanalyzer-stale-setjmp-buffer
	-Wanalyzer-tainted-allocation-size
	-Wanalyzer-tainted-array-index
	-Wanalyzer-tainted-assertion
	-Wanalyzer-tainted-divisor
	-Wanalyzer-tainted-offset
	-Wanalyzer-tainted-size
	-Wno-analyzer-too-complex
	-Wanalyzer-unsafe-call-within-signal-handler
	-Wanalyzer-use-after-free
	-Wanalyzer-use-of-pointer-in-stale-stack-frame
	-Wanalyzer-use-of-uninitialized-value
	-Wanalyzer-va-arg-type-mismatch
	-Wanalyzer-va-list-exhausted
	-Wanalyzer-va-list-leak
	-Wanalyzer-va-list-use-after-va-end
	-Wanalyzer-write-to-const
	-Wanalyzer-write-to-string-literal
)

if (build_type STREQUAL release)
	# Release Specific Flags
	target_compile_options(cmc-chess PRIVATE -O2 -flto -DNDEBUG)
elseif (build_type STREQUAL debug)
	#Debug Specific Flags
	target_compile_options(cmc-chess PRIVATE -O0 -g -DDEBUG)
endif()

