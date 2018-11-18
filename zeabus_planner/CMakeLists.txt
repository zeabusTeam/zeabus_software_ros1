cmake_minimum_required(VERSION 2.8.3)
project(zeabus_planner)

## Compile as C++11, supported in ROS Kinetic and newer
# add_compile_options(-std=c++11)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED rospy geometry_msgs std_msgs message_generation)

generate_messages( DEPENDENCIES std_msgs )

include_directories(
 include
 ${catkin_INCLUDE_DIRS}
)

catkin_package(
#  INCLUDE_DIRS include
  LIBRARIES zeabus_planner 
  CATKIN_DEPENDS rospy geometry_msgs std_msgs message_runtime
  DEPENDS system_lib 
)


