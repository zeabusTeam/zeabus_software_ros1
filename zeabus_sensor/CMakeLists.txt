cmake_minimum_required(VERSION 2.8.3)
project(zeabus_sensor)

find_package(catkin REQUIRED COMPONENTS
	roscpp
	zeabus_extension
	zeabus_serial
	std_msgs
	geometry_msgs
)

find_package(Boost REQUIRED COMPONENTS system)

include_directories( include ${catkin_INCLUDE_DIRS})

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES
  CATKIN_DEPENDS roscpp zeabus_extension zeabus_serial std_msgs geometry_msgs
  DEPENDS 
)

add_executable(port_dvl src/teledyne_dvl/dvl_port.cpp)
add_executable(node_dvl src/teledyne_dvl/dvl_node.cpp)

target_link_libraries(port_dvl ${catkin_LIBRARIES})
target_link_libraries(node_dvl ${catkin_LIBRARIES})

