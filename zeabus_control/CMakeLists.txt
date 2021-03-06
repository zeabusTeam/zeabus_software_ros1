cmake_minimum_required(VERSION 2.8.3)
project(zeabus_control)

find_package(catkin REQUIRED COMPONENTS	
	roscpp
	roslib
	nav_msgs
	std_msgs
	geometry_msgs
	zeabus_library
	zeabus_extension
	dynamic_reconfigure
)

find_package(Boost REQUIRED COMPONENTS system)

generate_dynamic_reconfigure_options(
	cfg/pid_.cfg
)

catkin_package(
#  INCLUDE_DIRS include
  LIBRARIES zeabus_control
  CATKIN_DEPENDS geometry_msgs nav_msgs roscpp std_msgs dynamic_reconfigure roslib 
  DEPENDS  Boost
)

include_directories(
 include
  ${catkin_INCLUDE_DIRS}
)

add_executable(front_control src/front_control.cpp)

add_dependencies( front_control	${PROJECT_NAME}_gencfg 
							${${PROJECT_NAME}_EXPORTED_TARGETS}
							${catkin_EXPORTED_TARGETS}
)
target_link_libraries( front_control ${catkin_LIBRARIES} ${Boost_LIBRARIES} )


add_executable(back_control src/back_control.cpp)

add_dependencies( back_control ${PROJECT_NAME}_gencfg 
							${${PROJECT_NAME}_EXPORTED_TARGETS}
							${catkin_EXPORTED_TARGETS}
)

target_link_libraries( back_control ${catkin_LIBRARIES} ${Boost_LIBRARIES} )
