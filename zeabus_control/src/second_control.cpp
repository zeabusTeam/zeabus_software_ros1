/*
	File name			:	second_control.cpp
	Author				:	Supasan Komonlit
	Date created		:	2019 , JAN 26
	Date last modified	:	2018 , ??? ??
	Purpose				:	For control system will give force in robot frame by pid
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.1.1
	status				:	Production

	Namespace			:	-
*/
// Standard header
#include	<stdio.h>
#include	<iostream>

#include	<ros/ros.h>
// Part of dynamic reconfigure of ros
#include	"second_control_function.cpp"

#include	<zeabus_library/ros.h>

#include	<zeabus_library/Odometry.h>
#include	<zeabus_library/Twist.h>

#include	<zeabus_library/text_color.h>

#include	<zeabus_library/rotation/rotation_handle.h>

#include	<zeabus_library/general.h>
#include	<zeabus_library/matrix.h>

#include	<zeabus_library/convert/Point3.h>

#include	<zeabus_library/control/pid.h>
#include	<zeabus_library/control/listen_odometry.h>
#include	<zeabus_library/control/listen_twist.h>

#define _PRINT_DATA_

int main( int argv , char** argc ){

	double ok_error[6] = { 0.01 , 0.01 , 0.1 , 0.02 , 0.02 , 0.02};
	double limit_pid[6] = { 1 , 1 , 3 , 1 , 1 , 1};

	ros::init( argv , argc , "second_control");

	ros::NodeHandle nh("");
	ros::NodeHandle ph("~");

	int temp_count[6];

	zeabus_library::rotation::RotationHandle rh;

	zeabus_library::control::ListenOdometry listen_odometry;
	zeabus_library::control::ListenTwist listen_twist( temp_count );

	zeabus_library::Point3 temp_position;
	zeabus_library::Point4 quaternion;
	zeabus_library::Point4 temp_quaternion;

	zeabus_library::Twist current;
	zeabus_library::Twist target;
	zeabus_library::Twist force;
	double current_velocity[6];
	double target_velocity[6];
	double force_velocity[6];
	double error_velocity[6];

	double temp_euler[3];

	listen_odometry.register_linear_velocity( &current.linear );
	listen_odometry.register_quaternion( &quaternion );
	listen_odometry.register_linear_position( &temp_position );
	listen_odometry.register_target_position( &temp_position );
	listen_odometry.register_target_quaternion( &temp_quaternion );
	listen_odometry.register_gyroscope( &current.angular );

	listen_twist.register_linear( &target.linear );
	listen_twist.register_angular( &target.angular );


	ros::Rate rate( 60 );

	ros::Publisher tell_force = nh.advertise< zeabus_library::Twist >("/control/force" , 10 );

	ros::Subscriber sub_current = nh.subscribe( "/localize/state" , 1 
							, &zeabus_library::control::ListenOdometry::callback
							, &listen_odometry );

	ros::Subscriber sub_target = nh.subscribe( "/control/target" , 1
							, &zeabus_library::control::ListenTwist::callback
							, &listen_twist );

	dynamic_reconfigure::Server<zeabus_control::pid_Config> server;
	dynamic_reconfigure::Server<zeabus_control::pid_Config>::CallbackType function;
	function = boost::bind(&dynamic_reconfigure_callback , _1 , _2 );
	server.setCallback( function );
	zeabus_library::DynParam tune_value;
	tune_value.set_package_file( "zeabus_control" );
	tune_value.set_directory_file( "constant" );
	tune_value.set_name_file( "second_control" );
	tune_value.set_node( "second_control" );

	zeabus_library::control::PID pid[6];

	rh.set_start_frame( 0 , 0 , 0 );
	boost::numeric::ublas::matrix<double> world_force;
	boost::numeric::ublas::matrix<double> robot_force;
	world_force.resize( 3 , 1 );
	robot_force.resize( 3 , 1 );

	for( int run = 0 ; run < 6 ; run++ ){
		pid[run].limit_i( limit_pid[run] );
		pid[run].set_constant( 0 , 0 , 0 );
		pid[run].offset_i(0);
	}

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
		zeabus_library::convert::Point3_to_array( current.linear , current_velocity , 0 );
		zeabus_library::convert::Point3_to_array( current.angular , current_velocity , 3 );
		zeabus_library::convert::Point3_to_array( target.linear , target_velocity , 0 );
		zeabus_library::convert::Point3_to_array( target.angular , target_velocity , 3 );
		for( int run = 0 ; run < 6 ; run++ ){
			error_velocity[run] = target_velocity[run] - current_velocity[run];
			if( zeabus_library::abs( error_velocity[run] ) < ok_error[ run ] )
				error_velocity[run] = 0 ;
			pid[run].calculate( error_velocity[run] , force_velocity[run] );		
		}
		zeabus_library::convert::array_to_Point3( force_velocity , force.angular , 3 );
		rh.set_target_frame( quaternion );
		zeabus_library::matrix::to_matrix( force_velocity , world_force );
		rh.start_rotation( world_force , robot_force );
		zeabus_library::matrix::to_array( robot_force , force_velocity );
		zeabus_library::convert::array_to_Point3( force_velocity , force.linear , 0 );

		tell_force.publish( force );
		#ifdef _PRINT_DATA_
			rh.target_frame.get_RPY( temp_euler[0] , temp_euler[1] , temp_euler[2] );	
			zeabus_library::clear_screen();
			printf("target_velocity : %8.4lf  %8.4lf  %8.4lf  %8.4lf  %8.4lf  %8.4lf\n\n"
				, target_velocity[0] , target_velocity[1] , target_velocity[2] 
				, target_velocity[3] , target_velocity[4] , target_velocity[5] );
			printf("current_velocity: %8.4lf  %8.4lf  %8.4lf  %8.4lf  %8.4lf  %8.4lf\n\n"
				, current_velocity[0] , current_velocity[1] , current_velocity[2] 
				, current_velocity[3] , current_velocity[4] , current_velocity[5] );
			printf("error_velocity  : %8.4lf  %8.4lf  %8.4lf  %8.4lf  %8.4lf  %8.4lf\n\n"
				, error_velocity[0] , error_velocity[1] , error_velocity[2] 
				, error_velocity[3] , error_velocity[4] , error_velocity[5] );
			printf("world_force     : %8.4lf  %8.4lf  %8.4lf  %8.4lf  %8.4lf  %8.4lf\n\n"
				, world_force( 0 , 0 ) , world_force( 1 , 0 ) , world_force( 2 , 0 ) 
				, force_velocity[3] , force_velocity[4] , force_velocity[5] );
			printf("robot_force     : %8.4lf  %8.4lf  %8.4lf  %8.4lf  %8.4lf  %8.4lf\n\n"
				, robot_force( 0 , 0 ) , robot_force( 1 , 0 ) , robot_force( 2 , 0 ) 
				, force_velocity[3] , force_velocity[4] , force_velocity[5] );
			printf("sending_force   : %8.4lf  %8.4lf  %8.4lf  %8.4lf  %8.4lf  %8.4lf\n\n"
				, force.linear.x , force.linear.y , force.linear.z 
				, force.angular.x , force.angular.y , force.angular.z );
			printf("current_euler   : %8.4lf  %8.4lf  %8.4lf\n\n"
				, temp_euler[0] , temp_euler[1] , temp_euler[2] );

		#endif
	}
}
