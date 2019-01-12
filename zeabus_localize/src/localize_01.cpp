/*
	File name			:	localize_01.cpp
	Author				:	Supasan Komonlit	
	Date created		:	2019 , JAN 09
	Date last modified	:	2019 , ??? ??
	Purpose				:	For localize by DVL , IMU 1 , Pressure

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	?.?.?
	status				:	Production

	Namespace			:	None
*/

#include	<stdio.h>

#include	<ros/ros.h>

#include	<zeabus_library/Odometry.h>

#include	<boost/numeric/ublas/matrix.hpp>

#include	<geometry_msgs/TwistWithCovarianceStamped.h>

#include	<zeabus_library/IMUQuaternion.h>

#include	<zeabus_library/rotation/rotation_handle.h>

#include	<zeabus_library/localize/listen_IMUQuaternion.h>

#include	<zeabus_library/localize/listen_DVL.h>

#include	<zeabus_library/localize/listen_pressure_nav.h>

#define _TEST_RECEVIE_VALUE

int main( int argv , char** argc ){

	ros::init( argv , argc , "localize" );

	ros::NodeHandle ph("~"); // Handle for manage param from launch file
	ros::NodeHandle nh(""); // Handle for manage about ths file in ros system

///////////////////////////////-- PARAMETER PART --/////////////////////////////////////////////
	std::string topic_output;
	std::string topic_imu;
	std::string topic_dvl;
	std::string topic_pressure;
	int frequency;

	ph.param< std::string >("topic_output" , topic_output , "/localize/state");
	ph.param< std::string >("topic_imu" , topic_imu , "/sensor/imu/node");
	ph.param< std::string >("topic_dvl" , topic_dvl , "/sensor/dvl/node");
	ph.param< std::string >("topic_pressure" , topic_pressure , "/sensor/pressure/node");
	ph.param< int >("frequency" , frequency , 50 );

////////////////////////////////////////////////////////////////////////////////////////////////

	ros::Rate rate( frequency );

	zeabus_library::Odometry message; // for send output
	zeabus_library::Point3 temp_linear_imu;

	zeabus_library::rotation::RotationHandle rotation_handle; 
		// for rotation and receive message imu to target_frame

	zeabus_library::localize::ListenIMUQuaternion listen_imu( &(message.pose.quaternion) 
															, &(message.velocity.angular) 
															, &(temp_linear_imu) );

	zeabus_library::localize::ListenPressureNav listen_pressure( &(message.pose.position.z) );
	printf("Position of message.pose.position.z is %x \n" , &(message.pose.position.z ) );
	zeabus_library::localize::ListenDVL listen_dvl( &(message.velocity.linear) );

////////////////////////////////////-- ROS SYSTEM --/////////////////////////////////////////////
	ros::Subscriber sub_imu = nh.subscribe( topic_imu , 1 
									, &zeabus_library::localize::ListenIMUQuaternion::callback
									, &listen_imu );
	ros::Subscriber sub_dvl = nh.subscribe( topic_dvl , 1 
									, &zeabus_library::localize::ListenDVL::callback
									, &listen_dvl );
	ros::Subscriber sub_pressure = nh.subscribe( topic_pressure , 1 
									, &zeabus_library::localize::ListenPressureNav::callback
									, &listen_pressure );

	ros::Publisher tell_auv_state = 
						nh.advertise< zeabus_library::Odometry >( topic_output , 1 );

	while( nh.ok() ){
		rate.sleep();
		ros::spinOnce();
		tell_auv_state.publish( message );
	} 
}
