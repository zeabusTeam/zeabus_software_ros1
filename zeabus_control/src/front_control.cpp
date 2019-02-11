/*
	File name			:	front_control.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2019 , FEB 06
	Date last modified	:	2019 , FEB 09
	Purpose				:	

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Maintainance

	Namespace			:	None
*/
//====================>

#define		_LOOK_TRANSFORM_
#define		_PRINT_DATA_

#include	<stdio.h>
#include	<iostream>

#include	<ros/ros.h>

#include	<zeabus_library/text_color.h>

#include	<zeabus_library/linear_equation.h>

#include	<zeabus_library/subscriber/SubOdometry.h>
#include	<zeabus_library/subscriber/SubTwistStamped.h>

#include	<zeabus_library/tf_handle/tf_quaternion.h>

#include	<zeabus_library/control/service_one_vector3_stamped.h>
#include	<zeabus_library/control/service_two_string_vector3_stamped.h>

#ifdef	_LOOK_TRANSFORM_
	#include	<tf/transform_broadcaster.h>
#endif

int main( int argv , char** argc ){

	ros::init( argv , argc , "/control/front");

	ros::NodeHandle ph("~");
	ros::NodeHandle nh("");

//====================> PARAMETER PART
	std::string state_topic;
	std::string twist_topic;
	std::string target_topic;
	std::string target_id;
	std::string my_id;
	std::string linear_id;
	std::string world_id;
	int frequency;
	int constant_ttl;
	int aborted_control;

	ph.param< std::string >("state_topic" , state_topic , "/localize/state" );
	ph.param< std::string >("twist_topic" , twist_topic , "/mission/twist" );
	ph.param< std::string >("target_topic" , target_topic , "/control/twist" );
	ph.param< std::string >("target_id" , target_id , "robot_target");
	ph.param< std::string >("world_id" , world_id , "world");
	ph.param< std::string >("linear_id" , linear_id , "robot_linear");
	ph.param< std::string >("my_id" , my_id , "control_front");
	ph.param< int >("frequency" , frequency , 40 );
	ph.param< int >("constant_ttl" , constant_ttl , 10 );

	aborted_control = copysign( frequency , -1 );

//====================> SETUP VARIABLE
	bool fix_velocity[6] = { 0, 0, 0, 0, 0, 0}; // for manage by service  
	double value_fix_velocity[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }; // use with fix_velocity
	double temp_euler[3];

	zeabus_library::tf_handle::TFQuaternion current_quaternion;
	zeabus_library::tf_handle::TFQuaternion target_quaternion;
	zeabus_library::tf_handle::TFQuaternion diff_quaternion;

	nav_msgs::Odometry current_state;
	nav_msgs::Odometry target_state; target_state.header.frame_id = target_id;

	nav_msgs::Odometry linear_state; linear_state.header.frame_id = linear_id;
	zeabus_library::LinearEquation lh; // Available only 2-D plan -- linear_handle

	geometry_msgs::TwistStamped control_twist; // output send to back_control
	geometry_msgs::TwistStamped received_twist; // receive twist from mission/twist
	int received_target_twist[6] = { 0, 0, 0, 0, 0, 0};

//====================> SETUP TF 
#ifdef _LOOK_TRANSFORM_
	static tf::TransformBroadcaster broadcaster;

	zeabus_library::tf_handle::TFQuaternion tf_quaternion;
	tf::Transform target_transform;
	tf::Transform linear_transform;
#endif
		
//====================> SETUP ROS SYSTEM
	ros::Rate rate( frequency );

	zeabus_library::control::ServiceOneVector3Stamped service_one;
	zeabus_library::control::ServiceTwoStringVector3Stamped service_two;

	service_one.register_all_state( &current_state , &target_state , &linear_state );
	service_one.register_equation( &lh );
	service_one.register_velocity( fix_velocity , value_fix_velocity );
	service_one.register_all_quaternion( &current_quaternion , &target_quaternion
			, &diff_quaternion );

	service_two.register_current( &target_state );
	service_two.register_target( &target_state );
	service_two.register_velocity( fix_velocity , value_fix_velocity );
	service_two.register_all_quaternion( &current_quaternion , &target_quaternion
			, &diff_quaternion );

	zeabus_library::subscriber::SubOdometry listener_state( &current_state );
	bool start_up = true; int received_state = 0; 
	listener_state.register_ttl( &received_state , 1 );
	ros::Subscriber sub_state = nh.subscribe( state_topic , 1 
			,&zeabus_library::subscriber::SubOdometry::callback_ttl , &listener_state );

	zeabus_library::subscriber::SubTwistStamped listerner_twist( &received_twist );
	listerner_twist.register_ttl( received_target_twist , 15 );
	ros::Subscriber sub_twist = nh.subscribe( twist_topic , 1 
			,&zeabus_library::subscriber::SubTwistStamped::callback_ttl , &listerner_twist );

	ros::Publisher tell_target = nh.advertise< geometry_msgs::TwistStamped>( target_topic , 1);

//====================> LOOP ROS OPERATOR
	while( nh.ok() ){
		rate.sleep();
		ros::spinOnce();
		if( start_up ){
			if( received_state ){
				tf_quaternion = current_state.pose.pose.orientation;
				tf_quaternion.get_RPY( temp_euler[0] , temp_euler[1] , temp_euler[3] );
				temp_euler[0] = 0.0;
				temp_euler[1] = 0.0;
				tf_quaternion.setEulerZYX( temp_euler[2] , temp_euler[1] , temp_euler[0] );
				current_state.pose.pose.orientation = tf_quaternion.get_quaternion();
				target_state = current_state;
				linear_state = current_state;
				start_up = false;
				received_state = 0;				
			}
			else{
				zeabus_library::normal_red("Warning can't receive current_state\n");		
			}
		}
		else{ // equation diff_quaternion * current_quaternion = target_quaternion			
			received_state--;
			if( received_state < aborted_control ){
				received_state = 0;
				start_up = false;
			}
		}
	}

}
