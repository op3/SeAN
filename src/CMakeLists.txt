cmake_minimum_required (VERSION 3.9 FATAL_ERROR)

include_directories("../include/")
add_library(sean_lib Absorption.cpp CrossSection.cpp Experiment.cpp InputReader.cpp Integrator.cpp PhononDensity.cpp Plotter.cpp Settings.cpp Target.cpp Writer.cpp)

target_link_libraries(sean_lib sean_options sean_warnings)

find_package(ROOT REQUIRED COMPONENTS MathMore)
target_link_libraries(sean_lib ROOT::MathMore ROOT::Hist ROOT::Gui)

find_package(FFTW REQUIRED)
target_link_libraries(sean_lib FFTW::Double)

option(USE_OPENMP "Use the OpenMP library for parallel computations (default: ON)." ON)
if(USE_OPENMP)
	find_package(OpenMP)
	if(OpenMP_CXX_FOUND)
        target_link_libraries(sean_lib OpenMP::OpenMP_CXX FFTW::DoubleOpenMP)
	endif()
endif()
