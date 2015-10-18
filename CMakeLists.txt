cmake_minimum_required(VERSION 2.8)

project(OpenCL_PA_2012)

include("cmake/FindCXXFeatures.cmake")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX11_COMPILER_FLAGS}")

if(APPLE)
  set(CMAKE_C_COMPILER clang)
  set(CMAKE_CXX_COMPILER clang++)
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD c++11)
  set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY libc++)
  set(CMAKE_OSX_ARCHITECTURES x86_64)
  set(CMAKE_LIBRARY_PREFIX /opt/local /usr/local)

  include_directories(
	  /opt/local/include
    /usr/local/include
    src/
    ${PROJECT_SOURCE_DIR}/thirdparty
  )

  find_library(COCOA_LIBRARY Cocoa)
  find_library(OPENGL_LIBRARY OpenGL)
  find_library(OPENAL_LIBRARY OpenAL)
  find_library(OPENCL_LIBRARY OpenCL)
  find_library(IOKIT_LIBRARY IOKit)

endif(APPLE)

# boost mess
if(WIN32 OR APPLE)
	set(Boost_USE_STATIC_LIBS ON)
endif(WIN32 OR APPLE)

find_package(Boost 1.54.0 REQUIRED COMPONENTS
  program_options
  filesystem
  system
  date_time
  chrono
)

if(NOT Boost_FOUND)
  message(FATAL_ERROR "Boost not found")
endif()

find_library(
	GLUT_LIB
	glut
)

add_executable(
  OpenCL_test
  ${PROJECT_SOURCE_DIR}/src/test/test.cpp
)

target_link_libraries(
  OpenCL_test
  ${Boost_LIBRARIES}
  ${OPENCL_LIBRARY}
)

add_executable(
  OpenCL_simple
  ${PROJECT_SOURCE_DIR}/src/simple/simple.cpp
  ${PROJECT_SOURCE_DIR}/src/simple/simple.cl
)

target_link_libraries(
  OpenCL_simple
  ${Boost_LIBRARIES}
  ${OPENCL_LIBRARY}
)

add_executable(
  OpenCL_julia
  ${PROJECT_SOURCE_DIR}/src/julia/cl_julia.cpp
  ${PROJECT_SOURCE_DIR}/src/julia/cl_julia.h
  ${PROJECT_SOURCE_DIR}/src/julia/cl_util.cpp
  ${PROJECT_SOURCE_DIR}/src/julia/cl_util.h
  ${PROJECT_SOURCE_DIR}/src/julia/glut_win.cpp
  ${PROJECT_SOURCE_DIR}/src/julia/glut_win.h
  ${PROJECT_SOURCE_DIR}/src/julia/julia.cl
  ${PROJECT_SOURCE_DIR}/src/julia/main.cpp
  ${PROJECT_SOURCE_DIR}/src/julia/win_julia.cpp
  ${PROJECT_SOURCE_DIR}/src/julia/win_julia.h
)

target_link_libraries(
  OpenCL_julia
  ${Boost_LIBRARIES}
  ${OPENGL_LIBRARY}
	${GLUT_LIB}
	${OPENCL_LIBRARY}
)

add_executable(
  OpenCL_floyd_warshall
  ${PROJECT_SOURCE_DIR}/src/floyd_warshall/cl_floyd_warshall.cpp
  ${PROJECT_SOURCE_DIR}/src/floyd_warshall/cl_floyd_warshall.h
  ${PROJECT_SOURCE_DIR}/src/floyd_warshall/cl_util.cpp
  ${PROJECT_SOURCE_DIR}/src/floyd_warshall/cl_util.h
  ${PROJECT_SOURCE_DIR}/src/floyd_warshall/ewd_file.cpp
  ${PROJECT_SOURCE_DIR}/src/floyd_warshall/ewd_file.h
  ${PROJECT_SOURCE_DIR}/src/floyd_warshall/floyd_warshall.cl
  ${PROJECT_SOURCE_DIR}/src/floyd_warshall/main.cpp
)

target_link_libraries(
  OpenCL_floyd_warshall
  ${Boost_LIBRARIES}
  ${OPENGL_LIBRARY}
	${GLUT_LIB}
	${OPENCL_LIBRARY}
)

add_executable(
  OpenCL_fft
  ${PROJECT_SOURCE_DIR}/src/fft/cl_fft.cpp
  ${PROJECT_SOURCE_DIR}/src/fft/cl_fft.h
  ${PROJECT_SOURCE_DIR}/src/fft/cl_util.cpp
  ${PROJECT_SOURCE_DIR}/src/fft/cl_util.h
  ${PROJECT_SOURCE_DIR}/src/fft/fft.cl
  ${PROJECT_SOURCE_DIR}/src/fft/fftw_fft.h
  ${PROJECT_SOURCE_DIR}/src/fft/main.cpp
  ${PROJECT_SOURCE_DIR}/src/fft/txt_file.h
)

target_link_libraries(
  OpenCL_fft
  ${Boost_LIBRARIES}
  ${OPENGL_LIBRARY}
	${GLUT_LIB}
	${OPENCL_LIBRARY}
)

add_executable(
  OpenCL_cellular_automata
  ${PROJECT_SOURCE_DIR}/src/cellular_automata/cl_cellular_automata.cpp
  ${PROJECT_SOURCE_DIR}/src/cellular_automata/cl_cellular_automata.h
  ${PROJECT_SOURCE_DIR}/src/cellular_automata/cl_util.cpp
  ${PROJECT_SOURCE_DIR}/src/cellular_automata/cl_util.h
  ${PROJECT_SOURCE_DIR}/src/cellular_automata/glut_win.cpp
  ${PROJECT_SOURCE_DIR}/src/cellular_automata/glut_win.h
  ${PROJECT_SOURCE_DIR}/src/cellular_automata/life.cl
  ${PROJECT_SOURCE_DIR}/src/cellular_automata/main.cpp
  ${PROJECT_SOURCE_DIR}/src/cellular_automata/win_cellular_automata.cpp
  ${PROJECT_SOURCE_DIR}/src/cellular_automata/win_cellular_automata.h
)

target_link_libraries(
  OpenCL_cellular_automata
  ${Boost_LIBRARIES}
  ${OPENGL_LIBRARY}
  ${GLUT_LIB}
  ${OPENCL_LIBRARY}
)
