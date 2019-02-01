cmake_minimum_required(VERSION 2.8.3)
project(zeabus_static)

find_package(catkin REQUIRED
	roscpp
	std_msgs
	geometry_msgs
	roslib
	sensor_msgs
	tf
)

find_package(Boost REQUIRED COMPONENTS
)

catkin_package(
	INCLUDE_DIRS include
	LIBRARIES zeabus_library
	CATKIN_DEPENDS roscpp message_runtime std_msgs geometry_msgs sensor_msgs tf
	DEPENDS Boost
)

include_directories(
	include	${catkin_INCLUDE_DIRS}
			${Boost_INCLUDE_DIRS}
)

set( zeabus_static_convert
	src/convert/TF_data.cpp)

add_library( zeabus_static STATIC		${zeabus_static_convert})

add_dependencies( zeabus_static			${${PROJECT_NAME}_EXPORTED_TARGETS}
										${catkin_EXPORTED_TARGETS} )

target_link_libraries( zeabus_static	${catkin_LIBRARIES} ${Boost_LIBRARIES})
