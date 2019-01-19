/*
	File name			:	first_control.cpp
	Author				:	Supasan Komonlit
	Date created		:	2018 , JAN 20
	Date last modified	:	2018 , ??? ??
	Purpose				:	For control system will give target velocity
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Production

	Namespace			:	-
*/

#include	<stdio.h>

#include	<ros/ros.h>

#include	<zeabus_library/euler.h>

#include	<zeabus_library/text_color.h>

#include	<zeabus_library/rotation/rotation_handle.h>

#include	<zeabus_library/Odometry.h>

#include	<zeabus_library/Twist.h>

#include	"first_control_function.cpp"

int main( int argv , char** argc ){

	ros::init( argv , argc , "first_control" );

	ros::NodeHandle ph("~"); // Handle for manage param from launch file == parameter handle
	ros::NodeHandle nh(""); // Handle for manage about this file in ros system == node handle

///////////////////////////////////-- PARAMETER PART --//////////////////////////////////////////
	std::string topic_state;
	std::string topic_output;
	int frequency;

	ph.param< std::string >("topic_state" , topic_state , "/localize/state");
	ph.param< std::string >("topic_output" ,topic_output , "/control/target" );
	ph.param< int >("frequency" , frequency , 50 );
///////////////////////////////////////////////////////////////////////////////////////////////

	ros::Rate rate( frequency );

	zeabus_library::Twist message; // for send target velocity to node second_control

	zeabus_library::Point3 current_position; // for collection current position		
	zeabus_library::Point3 target_position; // for collecting target position
	zeabus_library::Point3 temporary_position; // for collectin linear position for velocity
	clear_point3( current_position );
	clear_point3( target_position );
	clear_point3( temporary_position );

	zeabus_library::rotation::RotationHandle rh; // rotation handle
	zeabus_library::Point4 current_quaternion;
	zeabus_library::Point4 target_quaternion;
	clear_point4( current_quaternion );
	clear_point4( target_quaternion );

	
	while( nh.ok() ){
		rate.sleep();
		ros::spinOnce();
	}

}
