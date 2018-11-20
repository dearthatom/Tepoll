cmake_minimum_required(VERSION 2.8)
project(Tepoll)

set(CMAKE_CXX_STANDARD 11)

cmake_policy(SET CMP0053 OLD)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -O3 -g -std=c++11")

set(CMAKE_USE_RELATIVE_PATHS ON)

#find_package(OpenCV REQUIRED)

#
set(src_dir "@PROJECT_SOURCE_DIR@")
set(bin_dir "@PROJECT_BINARY_DIR@")
set(lib_dir "@PROJECT_BINARY_DIR@")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

set(SOLUTION_INCLUDE_DIRS
		${src_dir}/common
		${src_dir}/net_epoll
		)

add_subdirectory(common)
add_subdirectory(net_epoll)
add_subdirectory(proc)


