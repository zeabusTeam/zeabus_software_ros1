cmake_minimum_required(VERSION 2.8.3)
project(zeabus_extension)

find_package(catkin REQUIRED roslib roscpp )

find_package(Boost REQUIRED COMPONENTS system date_time)


## The catkin_package macro generates cmake config files for your package
## Declare things to be passed to dependent projects
## INCLUDE_DIRS: uncomment this if your package contains header files
## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
catkin_package(
  INCLUDE_DIRS include
  LIBRARIES zeabus_extension
  CATKIN_DEPENDS roscpp
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
)

add_library( zeabus_extension ${zeabus_extension_sources})

target_link_libraries( zeabus_extension
	${catkin_LIBRARIES}
	${Boost_LIBRARIES}
)

#for complie file cpp to execute file
add_executable(time_test src/test_folder/universal_day.cpp)
add_executable(posix_time_test src/test_folder/posix_time.cpp)
