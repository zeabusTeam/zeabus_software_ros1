cmake_minimum_required(VERSION 2.8.3)
project(zeabus_sensor)

find_package(catkin REQUIRED COMPONENTS
	roscpp
	zeabus_extension
	zeabus_serial
	std_msgs
	geometry_msgs
)

find_package(Boost REQUIRED COMPONENTS system chrono)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES zeabus_sensor
  CATKIN_DEPENDS roscpp zeabus_extension zeabus_serial std_msgs geometry_msgs
  DEPENDS Boost 
)

include_directories( include ${catkin_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})

set( zeabus_sensor_sources
	src/3dm_gx5_45/3dm_gx5_45_node.cpp
	src/3dm_gx5_45/imu_port.cpp
)

add_library( zeabus_sensor ${zeabus_sensor_sources})

add_executable(port_dvl src/teledyne_dvl/dvl_port.cpp)
add_executable(node_dvl src/teledyne_dvl/dvl_node.cpp)
add_executable(node_3dm_gx5_45 src/3dm_gx5_45/3dm_gx5_45_node.cpp)

target_link_libraries(port_dvl ${catkin_LIBRARIES})
target_link_libraries(node_dvl ${catkin_LIBRARIES})
target_link_libraries(node_3dm_gx5_45 ${catkin_LIBRARIES})
