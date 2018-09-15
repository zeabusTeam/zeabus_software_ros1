cmake_minimum_required(VERSION 2.8.3)

project(hg_ros_serial)

find_package(catkin REQUIRED COMPONENTS roscpp)
find_package(Boost REQUIRED COMPONENTS system chrono)

catkin_package(     
   INCLUDE_DIRS include
   CATKIN_DEPENDS roscpp   
   DEPENDS Boost 
   LIBRARIES hg_ros_serial
)

include_directories(
  include 
  ${Boost_INCLUDE_DIRS} 
  ${catkin_INCLUDE_DIRS}
)

add_library(hg_ros_serial src/serial.cpp)

