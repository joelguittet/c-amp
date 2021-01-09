# CMake minimum version
cmake_minimum_required(VERSION 3.14)

# Project name
project(c-amp)

# Project version
set(AMP_VERSION_MAJOR 0)
set(AMP_VERSION_MINOR 3)
set(AMP_VERSION_PATCH 1)

# Additional flags
set(c_flags "${c_flags} -Os -ffunction-sections -Wall -fPIC")
set(linker_flags "${linker_flags} -Wl,-gc-sections")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")

# Definitions
add_definitions(-DAMP_EXPORT_SYMBOLS -DAMP_API_VISIBILITY)

# Output directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)

# CMake subdirectories
if(NOT TARGET cjson)
  if(EXISTS lib/cJSON)
    add_subdirectory(lib/cJSON)
  endif()
endif()

# List of sources
file(GLOB_RECURSE src "src/*.c")

# Add include directories
include_directories(inc)
include_directories(lib/cJSON)

# Creation of the library
add_library(amp SHARED ${src})

# Link the library with the wanted libraries
target_link_libraries(amp cjson)

# Properties of the library
set_target_properties(amp
  PROPERTIES
  SOVERSION "${AMP_VERSION_MAJOR}"
  VERSION "${AMP_VERSION_MAJOR}.${AMP_VERSION_MINOR}.${AMP_VERSION_PATCH}"
)
        
# Creation of the examples binaries
option(ENABLE_AMP_EXAMPLES "Enable building amp examples" OFF)
if(ENABLE_AMP_EXAMPLES)
  add_executable(decoder "examples/decoder.c")
  target_link_libraries(decoder amp)
  add_executable(encoder "examples/encoder.c")
  target_link_libraries(encoder amp)
endif()

# Installation
set(CMAKE_INSTALL_FULL_LIBDIR lib)
set(CMAKE_INSTALL_FULL_BINDIR bin)
set(CMAKE_INSTALL_FULL_INCLUDEDIR include)
if(EXISTS lib/cJSON)
  install(FILES lib/cJSON/cJSON.h DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}")
  install(TARGETS cjson
    ARCHIVE DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_FULL_BINDIR}"
    INCLUDES DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}"
  )
endif()
install(FILES inc/amp.h DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}")
install(TARGETS amp
  ARCHIVE DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}"
  LIBRARY DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}"
  RUNTIME DESTINATION "${CMAKE_INSTALL_FULL_BINDIR}"
  INCLUDES DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}"
)
if(ENABLE_AMP_EXAMPLES)
  install(TARGETS decoder encoder
    ARCHIVE DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_FULL_BINDIR}"
    INCLUDES DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}"
  )
endif()
