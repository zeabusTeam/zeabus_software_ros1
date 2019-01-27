cmake_minimum_required(VERSION 2.8.3)
project(zeabus_mission)

find_package(catkin REQUIRED rospy zeabus_library std_msgs)

catkin_package(
#  INCLUDE_DIRS include
  LIBRARIES zeabus_mission
  CATKIN_DEPENDS rospy zeabus_library std_msgs
  DEPENDS system_lib
)

include_directories(
 include
 ${catkin_INCLUDE_DIRS}
)
