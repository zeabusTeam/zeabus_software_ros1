cmake_minimum_required(VERSION 2.8.3)
project(zeabus_planner)

find_package(catkin REQUIRED rospy geometry_msgs std_msgs message_generation nav_msgs tf)
catkin_python_setup()

generate_messages( DEPENDENCIES std_msgs )

include_directories(
 include
 ${catkin_INCLUDE_DIRS}
)

catkin_package(
#  INCLUDE_DIRS include
  LIBRARIES zeabus_planner 
  CATKIN_DEPENDS rospy geometry_msgs std_msgs message_runtime tf
  DEPENDS system_lib 
)
