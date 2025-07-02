# Required CMake version
cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_COMPILER "icpx")

# CMakeLists.txt for ISO2DFD_DPCPP project
project(iso2dfd_sycl)

# Set default build type to RelWithDebInfo if not specified
if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "Default CMAKE_BUILD_TYPE not set using Release with Debug Info")
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE
        STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel"
        FORCE)
endif()

# Set C++ flags and SYCL-specific flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fsycl -std=c++20 -fsycl-targets=nvptx64-nvidia-cuda -I../include")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fsycl -std=c++20 -I../include")
# Add the executable
add_executable(iso2dfd src/iso_2d_fd.cpp)

# Optionally, link OpenCL and SYCL if needed (uncomment if necessary)
# target_link_libraries(iso2dfd PRIVATE OpenCL sycl)

# Custom target to run the executable with arguments
add_custom_target(run
    COMMAND iso2dfd 1000 1000 2000
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)
