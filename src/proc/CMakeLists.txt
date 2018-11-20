cmake_minimum_required(VERSION 2.8)
project(mynet)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -O3 -g -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -O3 -g -std=c++11")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

include_directories(
		${SOLUTION_INCLUDE_DIRS}
	)
set(LIB_LIST pthread
        net_epoll
		common
        )
set(SRC_LIST
		main.cpp
		)

add_executable(epollserver ${SRC_LIST})
target_link_libraries(epollserver ${LIB_LIST})


#add_executable(test_client client.cpp)
#target_link_libraries(test_client ${LIB_LIST})
