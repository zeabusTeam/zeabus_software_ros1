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

#include	<zeabus_library/convert/Point3.h>

#include	<zeabus_library/control/listen_odometry.h>

#include	<zeabus_library/control/listen_twist.h>

#include	<zeabus_library/general.h>

#include	<zeabus_library/Twist.h>

#include	<zeabus_library/euler.h>

#include	"first_control_function.cpp"

#define _PRINT_DATA_

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
	int count_target_velocity[6];

	zeabus_library::Point3 current_position; // for collection current position		
	zeabus_library::Point3 diff_position;
	zeabus_library::Point3 target_position; // for collecting target position
	zeabus_library::Point3 temporary_position; // for collectin linear position for velocity
	zeabus_library::Point3 current_velocity; // for collecting linear velocity form localize
	zeabus_library::Point3 current_gyroscope; // for collecting angular velocity form localize
	clear_point3( current_position );
	clear_point3( target_position );
	clear_point3( temporary_position );

	zeabus_library::rotation::RotationHandle rh; // rotation handle
	zeabus_library::Point4 current_quaternion;
	zeabus_library::Point4 target_quaternion;
	double current_euler[3] = { 0 , 0 , 0 };
	double target_euler[3] = { 0 , 0 , 0 };
	double diff_euler[3] = { 0 , 0 , 0 };
	clear_point4( current_quaternion );
	clear_point4( target_quaternion );

	zeabus_library::control::ListenOdometry listen_odometry;
	listen_odometry.register_linear_position( &current_position );
	listen_odometry.register_quaternion( &current_quaternion );
	listen_odometry.register_linear_velocity( &current_velocity );
	listen_odometry.register_gyroscope( &current_gyroscope );

	zeabus_library::control::ListenTwist listen_twist( count_target_velocity );
	zeabus_library::Point3 target_velocity;
	zeabus_library::Point3 target_gyroscope;
	listen_twist.register_linear( &target_velocity );
	listen_twist.register_angular( &target_gyroscope );
	listen_twist.set_constant( 12 );
	
//////////////////////////////////////-- ROS SYSTEM --///////////////////////////////////////////
	ros::Subscriber sub_state = nh.subscribe( topic_state , 1 
								, &zeabus_library::control::ListenOdometry::callback
								, &listen_odometry );

	ros::Subscriber sub_target = nh.subscribe( "control/twist" , 1
								, &zeabus_library::control::ListenTwist::callback
								, &listen_twist );

	while( nh.ok() ){
		rate.sleep();
		ros::spinOnce();
		// for find diff euler
		rh.set_start_frame( current_quaternion ); 
		rh.set_target_frame( target_quaternion );
		rh.update_rotation();
		rh.start_frame.get_RPY( current_euler[0] , current_euler[1] , current_euler[2] );

		// case plan XY
		if( count_target_velocity[0] > 0 || count_target_velocity[1] > 0 ){
			target_position.x = current_position.x;
			target_position.y = current_position.y;
			message.linear.x = target_velocity.x * zeabus_library::euler::cos( current_euler[0] )
						+ target_velocity.y * zeabus_library::euler::cos( current_euler[1] + PI);
			message.linear.y = target_velocity.x * zeabus_library::euler::sin( current_euler[0] )
						+ target_velocity.y * zeabus_library::euler::sin( current_euler[1] + PI);
			count_target_velocity[0]--;
			count_target_velocity[1]--;
		}	
		else{
			diff_position.x = target_position.x - current_position.x;
			diff_position.y = target_position.y - current_position.y;
			if( check_zero( diff_position.x ) ) message.linear.x = 0;
			else if( diff_position.x < -1 ) message.linear.x = -1;
			else if( diff_position.x > 1 ) message.linear.x = 1;
			else if( diff_position.x < 0 ) message.linear.x = -0.05;
			else message.linear.x = 0.05;

			if( check_zero( diff_position.y ) ) message.linear.y = 0;
			else if( diff_position.y < -1 ) message.linear.y = -1;
			else if( diff_position.y > 1 ) message.linear.y = 1;
			else if( diff_position.y < 0 ) message.linear.y = -0.05;
			else message.linear.y = 0.05;
		}
	}

}
