cmake_minimum_required(VERSION 2.8.3)
project(zeabus_library)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  message_generation
)

add_service_files(
	DIRECTORY srv
	FILES
	check_position.srv
	get_target.srv
	one_point.srv
	two_point.srv
)

generate_messages( DEPENDENCIES std_msgs )

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES zeabus_library
  CATKIN_DEPENDS roscpp message_runtime std_msgs
#  DEPENDS system_lib
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

set( zeabus_control_sources
	src/zeabus_control/service_control.cpp
	src/zeabus_control/service_check_state.cpp
	src/zeabus_control/service_get_target.cpp
	src/zeabus_control/service_one_point.cpp
	src/zeabus_control/service_two_point.cpp
)

add_library( zeabus_library 
	${zeabus_control_sources}
)

add_dependencies( zeabus_library ${${PROJECT_NAME}_EXPORTED_TARGETS}
								 ${catkin_EXPORTED_TARGETS})

target_link_libraries( zeabus_library 
	${catkin_LIBRARIES}
)
