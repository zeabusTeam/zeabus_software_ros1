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
	Euler.msg
	Point3.msg
	Point4.msg
	IMUData.msg
	PoseEuler.msg
	ArrayFloat.msg
	IMUQuaternion.msg
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
	src/convert_Point3.cpp
	src/convert_Point4.cpp
	src/convert_Type2.cpp
	src/convert_State.cpp
	src/convert_IMUData.cpp
	src/convert_IMUQuaternion.cpp
	src/capture_time.cpp
	src/matrix.cpp
	src/euler.cpp
	src/vector.cpp
	src/error_code.cpp
)

set( zeabus_rotation_sources
	src/zeabus_rotation/quaternion.cpp
	src/zeabus_rotation/rotation_handle.cpp
	src/zeabus_rotation/convert_rotation.cpp
	src/zeabus_rotation/quaternion_handle.cpp
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
	src/zeabus_sensor/listen_IMUData.cpp
	src/zeabus_sensor/listen_IMUQuaternion.cpp
)

add_library( zeabus_library			STATIC		${zeabus_library_sources} 
												${zeabus_rotation_sources}
												${zeabus_sensor_sources}
												${zeabus_control_sources}
												${matrix_sources})
add_dependencies( zeabus_library				${${PROJECT_NAME}_EXPORTED_TARGETS}
												${catkin_EXPORTED_TARGETS} )
target_link_libraries( zeabus_library			${catkin_LIBRARIES} ${Boost_LIBRARIES})


add_executable( test_base_quaternion src_test/test_base_quaternion.cpp)
add_dependencies( test_base_quaternion ${${PROJECT_NAME}_EXPORTED_TARGETS}
										${catkin_EXPORTED_TARGETS} )
target_link_libraries( test_base_quaternion ${catkin_LIBRARIES} zeabus_library)

add_executable( test_color_text src_test/test_color_text.cpp)
add_dependencies( test_color_text ${${PROJECT_NAME}_EXPORTED_TARGETS}
										${catkin_EXPORTED_TARGETS} )
target_link_libraries( test_color_text ${catkin_LIBRARIES} zeabus_library)

add_executable( test_quaternion_handle src_test/test_quaternion_handle.cpp)
target_link_libraries( test_quaternion_handle ${catkin_LIBRARIES} zeabus_library)

add_executable( test_rotation_handle src_test/test_rotation_handle.cpp)
target_link_libraries( test_rotation_handle ${catkin_LIBRARIES} zeabus_library)
