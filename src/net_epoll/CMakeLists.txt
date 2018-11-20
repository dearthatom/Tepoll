
include_directories(
		${SOLUTION_INCLUDE_DIRS}
)

set(SRC_LIST
		epoll_server.cpp
		task.cpp
		tepoll.cpp
		)
#target_link_libraries(net_epoll SHARED ${SRC_LIST})
add_library(net_epoll SHARED ${SRC_LIST})