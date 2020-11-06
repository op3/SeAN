cmake_minimum_required (VERSION 3.9 FATAL_ERROR)

set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -Wconversion -Wsign-conversion -fopenmp")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -fopenmp")
set(CMAKE_CXX_FLAGS_RELWITHDEBUGINFO "-g ${CMAKE_CXX_FLAGS_RELEASE}")
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
	message(STATUS "Automatically set CMAKE_BUILD_TYPE to ${CMAKE_BUILD_TYPE}")
endif()

include_directories("../include/")
add_library(sean_lib Absorption.cpp CrossSection.cpp Experiment.cpp InputReader.cpp Integrator.cpp PhononDensity.cpp Plotter.cpp Settings.cpp Target.cpp Writer.cpp)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS}$)
find_package(ROOT REQUIRED COMPONENTS MathMore)
include(${ROOT_USE_FILE})
target_include_directories(sean_lib SYSTEM PUBLIC ${ROOT_INCLUDE_DIRS})
target_link_libraries(sean_lib ${ROOT_LIBRARIES})

find_package(FFTW REQUIRED)
target_link_libraries(sean_lib FFTW::Double)

option(USE_OPENMP "Use the OpenMP library for parallel computations (default: ON)." ON)
if(USE_OPENMP)
	find_package(OpenMP)
	if(OpenMP_CXX_FOUND)
        target_link_libraries(sean_lib OpenMP::OpenMP_CXX FFTW::DoubleOpenMP)
	endif()
endif()
