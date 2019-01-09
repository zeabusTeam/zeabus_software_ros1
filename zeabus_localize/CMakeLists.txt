cmake_minimum_required(VERSION 2.8.3)
project(zeabus_localize)

find_package(catkin REQUIRED COMPONENTS
  geometry_msgs
  roscpp
  sensor_msgs
  zeabus_library
)

find_package( Boost REQUIRED COMPONENTS

)

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES zeabus_localize
#  CATKIN_DEPENDS geometry_msgs roscpp sensor_msgs zeabus_library
#  DEPENDS system_lib
)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
# include
  ${catkin_INCLUDE_DIRS}
)
