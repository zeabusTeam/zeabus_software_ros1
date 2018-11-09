cmake_minimum_required(VERSION 2.8.3)
project(zeabus_control)

## Compile as C++11, supported in ROS Kinetic and newer
# add_compile_options(-std=c++11)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  geometry_msgs
  nav_msgs
  roscpp
  std_msgs
  zeabus_extension
  dynamic_reconfigure
  roslib
  message_generation
)

generate_dynamic_reconfigure_options(
	cfg/pid_control.cfg
	cfg/offset_control.cfg
)

add_message_files(
	FILES
	State.msg
	Point3.msg
	Type2.msg
	ArrayFloat.msg
)

add_service_files(
	DIRECTORY srv
	FILES
	check_position.srv
	get_target.srv
	one_point.srv
	two_point.srv
)

generate_messages( DEPENDENCIES std_msgs)

include_directories(
 include
  ${catkin_INCLUDE_DIRS}
)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES zeabus_control
  CATKIN_DEPENDS geometry_msgs nav_msgs roscpp std_msgs zeabus_extension dynamic_reconfigure message_runtime 
  DEPENDS system_lib
)

add_executable(control src/main_control.cpp)

add_dependencies( control ${PROJECT_NAME}_gencfg )

target_link_libraries( control ${catkin_LIBRARIES})
