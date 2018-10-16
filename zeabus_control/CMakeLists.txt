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
)

catkin_package(
#  INCLUDE_DIRS include
  LIBRARIES zeabus_control
  CATKIN_DEPENDS geometry_msgs nav_msgs roscpp std_msgs zeabus_extension
  DEPENDS system_lib
)

include_directories(
 include
  ${catkin_INCLUDE_DIRS}
)

add_executable(control src/main_control.cpp)

target_link_libraries( control ${catkin_LIBRARIES})
