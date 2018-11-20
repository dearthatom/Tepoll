
include_directories(
		${SOLUTION_INCLUDE_DIRS}
)
set(SRC_LIST
		common.cpp
		)
#target_link_libraries(common jpeg iconv)
add_library(common SHARED ${SRC_LIST})