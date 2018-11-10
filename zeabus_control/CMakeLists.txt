cmake_minimum_required(VERSION 2.8.3)
project(zeabus_control)

find_package(catkin REQUIRED COMPONENTS
  geometry_msgs
  nav_msgs
  roscpp
  std_msgs
  zeabus_extension
  dynamic_reconfigure
  roslib
  message_generation
  zeabus_library
)

find_package(Boost REQUIRED COMPONENTS system)

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

generate_messages( DEPENDENCIES std_msgs)


catkin_package(
#  INCLUDE_DIRS include
  LIBRARIES zeabus_control
  CATKIN_DEPENDS geometry_msgs nav_msgs roscpp std_msgs zeabus_extension dynamic_reconfigure message_runtime roslib 
  DEPENDS  Boost
)

include_directories(
 include
  ${catkin_INCLUDE_DIRS}
)

add_executable(control src/main_control.cpp)

add_dependencies( control ${PROJECT_NAME}_gencfg )

target_link_libraries( control ${catkin_LIBRARIES} ${Boost_LIBRARIES} )
