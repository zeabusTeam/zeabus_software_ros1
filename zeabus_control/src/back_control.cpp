/*
	File name			:	back_control.cpp
	Author				:	Supasan Komonlit
	Date created		:	2019 , FEB 10
	Date last modified	:	2018 , FEB 27
	Purpose				:	For control system will give force in robot frame by pid
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.3.0
	status				:	Production

	Namespace			:	-
*/
//===============>
// Standard header
#include	<stdio.h>
#include	<iostream>

#include	<ros/ros.h>
// Part of dynamic reconfigure of ros
#include	"tune_part.cpp"

#include	<zeabus_library/ros.h>

#include	<zeabus_library/text_color.h>

#include	<zeabus_library/control/pid.h>

#include	<geometry_msgs/TwistStamped.h>

#include	<zeabus_library/tf_handle/tf_quaternion.h>

#include	<zeabus_library/service/ServiceTwoBool.h>

#include	<zeabus_library/subscriber/SubTwistStamped.h>
#include	<zeabus_library/subscriber/SubOdometry.h>

#define _PRINT_DATA_

int main( int argv , char** argc ){

	double ok_error[6] = { 0.01 , 0.01 , 0.1 , 0.02 , 0.02 , 0.05};
	double limit_pid[6] = { 3 , 3 , 1.5 , 0.3 , 0.3 , 0.15 };
	double max_pid[6] = { 3.5 , 3.5 , 2.5 , 0.5 , 0.5 , 0.3 };
	double min_pid[6] = { -3.5 , -3.5 , 0 , -0.5 , -0.5 , -0.3};

	ros::init( argv , argc , "back_control");

	ros::NodeHandle nh("");
	ros::NodeHandle ph("~");

//===============> PARAMETER PART
	std::string state_topic;
	std::string target_topic;
	std::string output_topic;
	bool target_active; 
	bool current_active;
	bool target_free_xy = false;
	current_active = 0;
	int frequency;

	ph.param< std::string >("state_topic" , state_topic , "/localize/state" );
	ph.param< std::string >("target_topic" , target_topic , "/control/twist");
	ph.param< std::string >("output_topic" , output_topic , "/control/force");  	
	ph.param< int >("frequency" , frequency , 40 );
	ph.param< bool >("start_control" , current_active , false);
	target_active = current_active;

//===============> SETUP VARIABLE & ROS SYSTEM

	double temp_euler[3];
	
	nav_msgs::Odometry current_state;

	geometry_msgs::TwistStamped target_twist;
	geometry_msgs::TwistStamped diff_twist;
	geometry_msgs::TwistStamped temp_twist;
	geometry_msgs::TwistStamped force_twist;

	zeabus_library::subscriber::SubOdometry listen_state( &current_state );
	zeabus_library::subscriber::SubTwistStamped listen_target( &target_twist );

	zeabus_library::tf_handle::TFQuaternion qh; // quaternion handle
	zeabus_library::tf_handle::TFQuaternion qh_inverse; // quaternion handle

	ros::Rate rate( frequency );

	ros::Publisher tell_force = nh.advertise< geometry_msgs::TwistStamped >( output_topic , 1);

	ros::Subscriber sub_current = nh.subscribe( state_topic , 1 
							, &zeabus_library::subscriber::SubOdometry::callback
							, &listen_state );

	ros::Subscriber sub_target = nh.subscribe( target_topic , 1
							, &zeabus_library::subscriber::SubTwistStamped::callback
							, &listen_target );

	zeabus_library::service::ServiceTwoBool service_active; 
	service_active.register_bool( &target_active );

	ros::ServiceServer ser_active_control = nh.advertiseService("/control/active"
			, &zeabus_library::service::ServiceTwoBool::callback
			, &service_active );

	zeabus_library::service::ServiceTwoBool service_free_xy;
	service_free_xy.register_bool( &target_free_xy );

	ros::ServiceServer ser_free_xy = nh.advertiseService("/control/free_xy" 
			, &zeabus_library::service::ServiceTwoBool::callback
			, &service_free_xy );


//===============> DYNAMIC_RECONFIGURE SYSTEM & LIBRARY FOR SAVE & LOAD
	dynamic_reconfigure::Server< zeabus_control::pid_Config > server;
	dynamic_reconfigure::Server< zeabus_control::pid_Config >::CallbackType function;
	function = boost::bind(&dynamic_reconfigure_callback , _1 , _2 );
	server.setCallback( function );
	zeabus_library::DynParam tune_value;
	tune_value.set_package_file( "zeabus_control" );
	tune_value.set_directory_file( "constant" );
	tune_value.set_name_file( "back_control" );
	tune_value.set_node( "back_control" );

	zeabus_library::control::PID pid[6];

	for( int run = 0 ; run < 6 ; run++ ){
		pid[run].limit_i( limit_pid[run] );
		pid[run].set_constant( 0 , 0 , 0 );
		pid[run].set_limit( min_pid[run] , max_pid[run] );
	}

//===============> LOOP OPERATING ROS SYSTEM
	while( nh.ok() ){
		rate.sleep();
		ros::spinOnce();
		if( ! already_loading_constant ){
			already_loading_constant = true;
			tune_value.load();
		}
		else if( want_save_constant ){
			tune_value.dump();
			reset_constant( pid );	
			want_save_constant = false;
		}
		if( current_active ){ // now active mode
			if( ! target_active ){ // data want to close activate
				zeabus_library::clear_screen();
				zeabus_library::bold_red("<========== NON ACTIVE CONTROL ==========>");
				current_active = false;
			}
		}
		else{ // now non active mode
			if( target_active ){ // data want to activate control
				zeabus_library::clear_screen();
				zeabus_library::bold_green("<========== ACTIVE CONTROL ==========>");
				current_active = true;
				reset_constant( pid ); 
			}
			else{ 
				continue;
			}
		}

		if( target_free_xy ){
			target_twist.twist.linear.x = current_state.twist.twist.linear.x;
			target_twist.twist.linear.y = current_state.twist.twist.linear.y;
		}

		diff_twist.twist.linear.x = 
				target_twist.twist.linear.x - current_state.twist.twist.linear.x;
		pid[0].calculate( diff_twist.twist.linear.x , temp_twist.twist.linear.x);

		diff_twist.twist.linear.y = 
				target_twist.twist.linear.y - current_state.twist.twist.linear.y;
		pid[1].calculate( diff_twist.twist.linear.y , temp_twist.twist.linear.y);

		diff_twist.twist.linear.z = 
				target_twist.twist.linear.z - current_state.twist.twist.linear.z;
		pid[2].calculate( diff_twist.twist.linear.z , temp_twist.twist.linear.z);

		diff_twist.twist.angular.x = 
				target_twist.twist.angular.x - current_state.twist.twist.angular.x;
		pid[3].calculate( diff_twist.twist.angular.x , temp_twist.twist.angular.x);

		diff_twist.twist.angular.y = 
				target_twist.twist.angular.y - current_state.twist.twist.angular.y;
		pid[4].calculate( diff_twist.twist.angular.y , temp_twist.twist.angular.y);

		diff_twist.twist.angular.z = 
				target_twist.twist.angular.z - current_state.twist.twist.angular.z;
		pid[5].calculate( diff_twist.twist.angular.z , temp_twist.twist.angular.z);

		qh = current_state.pose.pose.orientation;

		qh_inverse = qh.inverse();

		force_twist.twist.linear = qh_inverse.rotation( temp_twist.twist.linear );
		force_twist.twist.angular = temp_twist.twist.angular;
		
		tell_force.publish( force_twist );
		#ifdef _PRINT_DATA_
			zeabus_library::clear_screen();
			printf("CURRENT_EULER    :"); qh.print_radian(); printf("\n");
			printf("CURRENT_VELOCITY :%10.4lf%10.4lf%10.4lf%10.4lf%10.4lf%10.4lf\n" 
					, current_state.twist.twist.linear.x , current_state.twist.twist.linear.y
					, current_state.twist.twist.linear.z , current_state.twist.twist.angular.x
					, current_state.twist.twist.angular.y , current_state.twist.twist.angular.z);
			printf("TARGET_VELOCITY  :%10.4lf%10.4lf%10.4lf%10.4lf%10.4lf%10.4lf\n" 
					, target_twist.twist.linear.x , target_twist.twist.linear.y 
					, target_twist.twist.linear.z , target_twist.twist.angular.x 
					, target_twist.twist.angular.y , target_twist.twist.angular.z );
			printf("DIFF_TWIST       :%10.4lf%10.4lf%10.4lf%10.4lf%10.4lf%10.4lf\n" 
					, diff_twist.twist.linear.x , diff_twist.twist.linear.y 
					, diff_twist.twist.linear.z , diff_twist.twist.angular.x 
					, diff_twist.twist.angular.y , diff_twist.twist.angular.z );
			printf("TEMP_TWIST       :%10.4lf%10.4lf%10.4lf%10.4lf%10.4lf%10.4lf\n" 
					, temp_twist.twist.linear.x , temp_twist.twist.linear.y 
					, temp_twist.twist.linear.z , temp_twist.twist.angular.x 
					, temp_twist.twist.angular.y , temp_twist.twist.angular.z );
			printf("FORCE_TWIST      :%10.4lf%10.4lf%10.4lf%10.4lf%10.4lf%10.4lf\n" 
					, force_twist.twist.linear.x , force_twist.twist.linear.y 
					, force_twist.twist.linear.z , force_twist.twist.angular.x 
					, force_twist.twist.angular.y , force_twist.twist.angular.z);
		#endif
	}
}

