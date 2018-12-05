cmake_minimum_required(VERSION 2.8.3)
project(zeabus_library)

find_package( catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  geometry_msgs
  message_generation
)

find_package( Boost REQUIRED COMPONENTS

)

add_message_files(
	FILES
	State.msg
	Type2.msg
	Point3.msg
	ArrayFloat.msg
	Euler.msg
	PoseEuler.msg
	IMUData.msg
)

add_message_files(
	DIRECTORY msg/zeabus_navigator
	FILES
	ObjectResult.msg
)

add_service_files(
	DIRECTORY srv/zeabus_control
	FILES
	OnePoint.srv
	TwoPoint.srv
	GetTarget.srv
	NumberService.srv
	CheckPosition.srv
	SurveyRequest.srv
	
)

add_service_files(
	DIRECTORY srv/zeabus_planner
	FILES
	MissionResult.srv
)


add_service_files(
	DIRECTORY srv/zeabus_navigator
	FILES
	GetObject.srv
	SetObject.srv
	DelObject.srv
)

generate_messages( DEPENDENCIES std_msgs geometry_msgs)

catkin_package(
	INCLUDE_DIRS include
	LIBRARIES zeabus_library
	CATKIN_DEPENDS roscpp message_runtime std_msgs
	DEPENDS Boost
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
)

set( zeabus_library_sources
	src/convert_bytes.cpp
	src/convert_rotation.cpp
)

set( zeabus_control_sources
	src/zeabus_control/service_control.cpp
	src/zeabus_control/service_check_state.cpp
	src/zeabus_control/service_get_target.cpp
	src/zeabus_control/service_one_point.cpp
	src/zeabus_control/service_two_point.cpp
	src/zeabus_control/service_number_service.cpp
	src/zeabus_control/service_survey_request.cpp
	src/zeabus_control/listen_odometry.cpp
	src/zeabus_control/listen_twist.cpp
	src/zeabus_control/find_error_state_double.cpp
	src/zeabus_control/find_robot_error_double.cpp
	src/zeabus_control/find_robot_force_double.cpp
	src/zeabus_control/sum_pid_bound_id.cpp
)

set( zeabus_sensor_sources
	src/zeabus_sensor/serial_port.cpp
	src/zeabus_sensor/base_port.cpp
	src/zeabus_sensor/lord_microstrain.cpp	
	src/zeabus_sensor/synchronous_port.cpp
)

add_library( zeabus_library	STATIC	${zeabus_control_sources} 
									${zeabus_sensor_sources}
									${zeabus_library_sources})

add_dependencies( zeabus_library	${${PROJECT_NAME}_EXPORTED_TARGETS}
									${catkin_EXPORTED_TARGETS} )

target_link_libraries( zeabus_library	${catkin_LIBRARIES} ${Boost_LIBRARIES})
