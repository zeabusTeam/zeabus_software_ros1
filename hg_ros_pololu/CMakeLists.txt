cmake_minimum_required(VERSION 2.8.3)

project(hg_ros_pololu)

find_package(catkin REQUIRED COMPONENTS roscpp hg_ros_serial std_msgs std_srvs message_generation)

add_message_files(
  DIRECTORY msg
  FILES Pwm.msg
)

generate_messages(DEPENDENCIES std_msgs std_srvs)

catkin_package(     
   DEPENDS Boost 
   CATKIN_DEPENDS roscpp hg_ros_serial std_msgs std_srvs message_runtime
   INCLUDE_DIRS include
   LIBRARIES hg_ros_pololu
)

include_directories(
  include    
  ${catkin_INCLUDE_DIRS}    
)

add_library(hg_ros_pololu 
  src/maestro.cpp
)

add_executable(maestro_node src/maestro_node.cpp)
target_link_libraries(maestro_node  
  hg_ros_pololu
  ${catkin_LIBRARIES}
  ${Boost_LIBRARIES}
)
