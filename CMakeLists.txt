cmake_minimum_required(VERSION 3.10)

# Project name
project(FPFDisplay LANGUAGES CXX)

# Find ROOT package
# This command will try to find ROOT and set up necessary variables
# like ROOT_INCLUDE_DIRS and ROOT_LIBRARIES.
# You might need to have your ROOT environment sourced (e.g., by running `source /path/to/root/bin/thisroot.sh`)
# before running cmake, or have ROOT installed in a standard location.
find_package(ROOT REQUIRED COMPONENTS Core RIO Graf Graf3d Gpad Gui Geom Eve RGL)

if(ROOT_FOUND)
    message(STATUS "ROOT_INCLUDE_DIRS = ${ROOT_INCLUDE_DIRS}")
    message(STATUS "ROOT_LIBRARIES   = ${ROOT_LIBRARIES}")
else()
    message(FATAL_ERROR "ROOT not found. Please ensure ROOT is installed and sourced correctly.")
endif()

# Find local sources and headers
file(GLOB_RECURSE sources
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

# Build GUIDisplay dictiornary
include(${ROOT_USE_FILE})
ROOT_GENERATE_DICTIONARY(GUIDisplayDict
    ${CMAKE_CURRENT_SOURCE_DIR}/include/GUIDisplay.hh
    LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/include/LinkDef.h
)

# Add the executable
add_executable(FPFDisplay ${CMAKE_CURRENT_SOURCE_DIR}/FPFDisplay.cpp ${sources} ${CMAKE_CURRENT_BINARY_DIR}/GUIDisplayDict.cxx)

# Include ROOT + local headers
target_include_directories(FPFDisplay PRIVATE ${ROOT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link against ROOT libraries
target_link_libraries(FPFDisplay PRIVATE ${ROOT_LIBRARIES})