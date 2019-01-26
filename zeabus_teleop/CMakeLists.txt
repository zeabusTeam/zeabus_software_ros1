cmake_minimum_required(VERSION 2.8.3)
project(zeabus_teleop)
## Find catkin macros and libraries
find_package(catkin REQUIRED COMPONENTS roscpp geometry_msgs joy zeabus_library)

include_directories(${catkin_INCLUDE_DIRS})

catkin_package(
	INCLUDE_DIRS
	LIBRARIES
	CATKIN_DEPENDS roscpp geometry_msgs joy
	DEPENDS
)

###########
## Build ##
###########
add_executable(zeabus_joy src/zeabus_joy.cpp)
target_link_libraries(zeabus_joy ${catkin_LIBRARIES})

add_executable(node_joy src/joy_node.cpp)
target_link_libraries(node_joy ${catkin_LIBRARIES})
#############
## Install ##
#############
## Mark executable scripts (Python etc.) for installation
install(PROGRAMS	
	DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)
## Mark executables and/or libraries for installation
install(TARGETS zeabus_joy
	RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)
## Mark all other useful stuff for installation
install(DIRECTORY launch
	DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)
