cmake_minimum_required(VERSION 2.8.3)
project(zeabus_elec_ros_hardware_interface)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  message_generation
  nav_msgs
  std_msgs
  zeabus_elec_ros_peripheral_bridge
)

add_service_files(FILES
    IOCommand.srv
    PowerSwitchCommand.srv
	Torpedo.srv
)

generate_messages(DEPENDENCIES
    nav_msgs
    std_msgs
)

catkin_package(
  CATKIN_DEPENDS roscpp message_runtime nav_msgs std_msgs zeabus_elec_ros_peripheral_bridge
)
include_directories(
  ${catkin_INCLUDE_DIRS}
)

add_executable(zeabus_elec_ros_hardware_interface src/zeabus_elec_ros_hardware_interface.cpp)
add_dependencies(zeabus_elec_ros_hardware_interface ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(zeabus_elec_ros_hardware_interface ${catkin_LIBRARIES})
