cmake_minimum_required(VERSION 2.8.3)
project(zeabus_master)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  zeabus_library
)

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES zeabus_master
  CATKIN_DEPENDS roscpp std_msgs zeabus_library
#  DEPENDS system_lib
)

include_directories(
# include
  ${catkin_INCLUDE_DIRS}
)

add_executable( master_switch src/master_switch.cpp)
add_dependencies( master_switch ${PROJECT_NAME}_EXPORTED_TARGETS ${catkin_EXPORTED_TARGETS})
target_link_libraries( master_switch ${catkin_LIBRARIES} )
