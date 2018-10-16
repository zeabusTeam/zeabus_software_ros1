cmake_minimum_required(VERSION 2.8.3)
project(zeabus_extension)

find_package(catkin REQUIRED roslib roscpp )

find_package(Boost REQUIRED COMPONENTS system date_time thread chrono)


## The catkin_package macro generates cmake config files for your package
## Declare things to be passed to dependent projects
## INCLUDE_DIRS: uncomment this if your package contains header files
## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
catkin_package(
  INCLUDE_DIRS include
  LIBRARIES zeabus_extension
  CATKIN_DEPENDS roscpp roslib 
  DEPENDS Boost
)

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
 include
 ${catkin_INCLUDE_DIRS}
 ${Boost_INCLUDE_DIRS}
)

set( zeabus_extension_sources
	src/convert_to_string.cpp
	src/world_time.cpp
	src/precise_time.cpp
	src/log_data.cpp
	src/ros.cpp
	src/generate_message.cpp
	src/manage_port.cpp
	src/find_error_position.cpp
)

add_library( zeabus_extension ${zeabus_extension_sources})

target_link_libraries( zeabus_extension
	${catkin_LIBRARIES}
	${Boost_LIBRARIES}
)

install( TARGETS zeabus_extension 
	ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
	LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
	RUNTIME DESTINATION ${CATKIN_GLOBAL_LIB_DESTINATION}
)

#install(
#	DIRECTORY include/${PROJECT_NAME}
#	DESTINARION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
#)
