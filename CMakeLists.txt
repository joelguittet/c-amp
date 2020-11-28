# CMake minimum version
cmake_minimum_required(VERSION 3.5)

# Additional flags
set(c_flags "${c_flags} -Os -ffunction-sections -Wall -fPIC")
set(linker_flags "${linker_flags} -Wl,-gc-sections")

# Output directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)

# List of sources
file(GLOB_RECURSE src "src/*.c")

# Add include directories
include_directories(inc)

# Creation of the library
add_library(amp SHARED ${src})

# Creation of the examples binaries
add_custom_target(amp_examples)
add_executable(decoder EXCLUDE_FROM_ALL "examples/decoder.c")
target_link_libraries(decoder amp)
add_dependencies(amp_examples decoder)
add_executable(encoder EXCLUDE_FROM_ALL "examples/encoder.c")
target_link_libraries(encoder amp)
add_dependencies(amp_examples encoder)
