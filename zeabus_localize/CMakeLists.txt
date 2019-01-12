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
  LIBRARIES zeabus_localize
  CATKIN_DEPENDS geometry_msgs roscpp sensor_msgs zeabus_library
  DEPENDS 
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

add_executable( localize_01 src/localize_01.cpp)

add_dependencies( localize_01 ${PROJECT_NAME}_gencfg 
							${${PROJECT_NAME}_EXPORTED_TARGETS}
							${catkin_EXPORTED_TARGEtS}
)

target_link_libraries( localize_01 ${catkin_LIBRARIES} ${Boost_LIBRARIES})
