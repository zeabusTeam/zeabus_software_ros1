cmake_minimum_required(VERSION 2.8.3)
project(zeabus_sensor)

find_package(catkin REQUIRED COMPONENTS
	roscpp
	zeabus_extension
	std_msgs
	geometry_msgs
	message_generation
	
)

find_package(Boost REQUIRED COMPONENTS system chrono)

add_service_files(
	DIRECTORY srv
	FILES
	port_sensor.srv
)

# HAVE THIS LINE FOR GENERATE MESSAGE
generate_messages( DEPENDENCIES std_msgs)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES zeabus_sensor
  CATKIN_DEPENDS roscpp zeabus_extension  std_msgs geometry_msgs message_runtime
  DEPENDS Boost 
)

include_directories( include ${catkin_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})

#add_executable(node_3dm_gx5_45 src/3dm_gx5_45/3dm_gx5_45_node.cpp)
#add_executable(node_filter_imu src/3dm_gx5_45/filter_imu_node.cpp)

#add_dependencies(node_3dm_gx5_45 ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTD_TARGETS})

#target_link_libraries(node_3dm_gx5_45 ${catkin_LIBRARIES} ${boost_LIBRARIES})
#target_link_libraries(node_filter_imu ${catkin_LIBRARIES} ${boost_LIBRARIES})
