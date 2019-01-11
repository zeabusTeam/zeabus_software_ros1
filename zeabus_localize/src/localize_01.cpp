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

#include	<zeabus_library/IMUQuaternion.h>

#include	<zeabus_library/rotation/rotation_handle.h>

#include	<zeabus_library/localize/IMUQuaternion.h>

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
	ph.param< std::string >("topic_pressure" , topic_pressure , "/sensor/pressure");
	ph.param< int >("frequency" , frequency , 50 );

////////////////////////////////////////////////////////////////////////////////////////////////

	ros::Rate rate( frequency );

	zeabus_library::Odometry message;
	zeabus_library::rotation::RotationHandle rotation_handle;
}
