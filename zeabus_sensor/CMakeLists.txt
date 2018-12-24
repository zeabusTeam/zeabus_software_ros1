cmake_minimum_required(VERSION 2.8.3)
project(zeabus_sensor)

find_package(catkin REQUIRED COMPONENTS
	roscpp
	sensor_msgs
	zeabus_library
)

find_package(Boost REQUIRED COMPONENTS system chrono)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES zeabus_sensor  
  CATKIN_DEPENDS roscpp  zeabus_library sensor_msgs
  DEPENDS Boost 
)

include_directories( include ${catkin_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})

add_executable( imu_port src/inertial_measurement_unit/imu_port.cpp )
add_dependencies( imu_port ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries( imu_port ${catkin_LIBRARIES} ${boost_LIBRARIES})

#add_executable( imu_node src/inertial_measurement_unit/imu_node.cpp )
#add_dependencies( imu_node ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
#target_link_libraries( imu_node ${catkin_LIBRARIES} ${boost_LIBRARIES})
