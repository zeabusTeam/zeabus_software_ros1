cmake_minimum_required(VERSION 2.8.3)
project(zeabus_library)

find_package( catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  geometry_msgs
  message_generation
  roslib
)

find_package( Boost REQUIRED COMPONENTS

)

add_message_files(
	FILES
	Pose.msg
	Twist.msg
	State.msg
	Type2.msg
	Euler.msg
	Point3.msg
	Point4.msg
	IMUData.msg
	Odometry.msg
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

set( zeabus_library_convert_sources
	src/convert/general.cpp
	src/convert/bytes.cpp
	src/convert/Point3.cpp
	src/convert/Point4.cpp
	src/convert/Type2.cpp
	src/convert/State.cpp
	src/convert/IMUData.cpp
	src/convert/boost_time.cpp
	src/convert/IMUQuaternion.cpp
)

set( zeabus_library_localize_sources
	src/localize/listen_DVL.cpp
	src/localize/listen_DVL_geometry.cpp
	src/localize/listen_pressure_nav.cpp
	src/localize/listen_IMUQuaternion.cpp
)

set( zeabus_library_control_sources
	src/control/listen_odometry.cpp	
	src/control/listen_twist.cpp	
)

set( zeabus_library_rotation_sources
	src/rotation/quaternion.cpp
	src/rotation/rotation_handle.cpp
	src/rotation/matrix_rotation.cpp
	src/rotation/quaternion_handle.cpp
)

set( zeabus_library_sensor_sources
	src/sensor/serial_port.cpp
	src/sensor/base_port.cpp
	src/sensor/string_port.cpp
	src/sensor/lord_microstrain.cpp	
	src/sensor/synchronous_port.cpp
	src/sensor/listen_IMUData.cpp
	src/sensor/listen_IMUQuaternion.cpp
)

set( zeabus_library_sources
	src/capture_time.cpp
	src/matrix.cpp
	src/euler.cpp
	src/vector.cpp
	src/error_code.cpp
	src/file.cpp
	src/listen_basic.cpp
	src/text_color.cpp
	src/general.cpp
	src/linear_equation.cpp
)

add_library( zeabus_library			STATIC		${zeabus_library_sources} 
												${zeabus_library_sensor_sources}
												${zeabus_library_convert_sources}
												${zeabus_library_control_sources}
												${zeabus_library_localize_sources}
												${zeabus_library_rotation_sources} )
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

#add_executable( test_quaternion_handle src_test/test_quaternion_handle.cpp)
#target_link_libraries( test_quaternion_handle ${catkin_LIBRARIES} zeabus_library)

add_executable( test_rotation_handle src_test/test_rotation_handle.cpp)
target_link_libraries( test_rotation_handle ${catkin_LIBRARIES} zeabus_library)
