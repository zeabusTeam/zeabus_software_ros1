cmake_minimum_required(VERSION 2.8.3)
project(zeabus_sensor)

find_package(catkin REQUIRED COMPONENTS
	tf
	roscpp
	std_msgs
	sensor_msgs
	geometry_msgs
	zeabus_static
	zeabus_library
)

find_package(Boost REQUIRED COMPONENTS system chrono)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES zeabus_sensor  
  CATKIN_DEPENDS roscpp  zeabus_library sensor_msgs tf geometry_msgs zeabus_static std_msgs
  DEPENDS Boost 
)

include_directories( include ${catkin_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})

add_executable( imu_port src/inertial_measurement_unit/imu_port.cpp )
add_dependencies( imu_port ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries( imu_port ${catkin_LIBRARIES} ${boost_LIBRARIES})

add_executable( imu_node src/inertial_measurement_unit/imu_node.cpp )
add_dependencies( imu_node ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries( imu_node ${catkin_LIBRARIES} ${boost_LIBRARIES})

add_executable( dvl_port src/teledyne_dvl/dvl_port.cpp )
add_dependencies( dvl_port ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries( dvl_port ${catkin_LIBRARIES} ${boost_LIBRARIES})

add_executable( dvl_node src/teledyne_dvl/dvl_node.cpp )
add_dependencies( dvl_node ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries( dvl_node ${catkin_LIBRARIES} ${boost_LIBRARIES})

# ------------------------------------- TF FILE ------------------------------------------------

add_executable( tf_port_imu src/inertial_measurement_unit/tf_port_imu.cpp )
add_dependencies( tf_port_imu ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries( tf_port_imu ${catkin_LIBRARIES} ${boost_LIBRARIES})

add_executable( tf_port_dvl src/teledyne_dvl/tf_port_dvl.cpp )
add_dependencies( tf_port_dvl ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries( tf_port_dvl ${catkin_LIBRARIES} ${boost_LIBRARIES})

