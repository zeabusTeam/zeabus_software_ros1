////////////////////////////////////////// DETAIL ///////////////////////////////////////////////
//
//	File		: head_control.cpp 
//	Purpose		: include all file to collect variable class and all function
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Oct 18
//
//
//
///////////////////////////////////////// END PART //////////////////////////////////////////////

#include	<iostream> // standard of cpp
#include	<ros/ros.h> // standard of ros system
#include	<cstdlib>

// include for dynamic reconfigure of ros
#include	<dynamic_reconfigure/server.h>
#include	<zeabus_control/pid_controlConfig.h>

// include zeabus library when sucess to shared
#include	<zeabus_extension/log_data.h>

// include message for use in main_control
#include	<nav_msgs/Odometry.h>
#include	<geometry_msgs/Twist.h>
#include	<zeabus_library/Point3.h>
#include	<zeabus_library/Type2.h>
#include	<zeabus_library/State.h>

#include	<zeabus_library/zeabus_control/service_one_point.h>
#include	<zeabus_library/zeabus_control/service_two_point.h>
#include	<zeabus_library/zeabus_control/service_check_state.h>
#include	<zeabus_library/zeabus_control/service_get_target.h>

#include	<zeabus_library/zeabus_control/listen_twist.h> 
			// Class object for listenning twist message
#include	<zeabus_library/zeabus_control/listen_odometry.h> 
			// Class object for listenning auv status
#include	<zeabus_library/zeabus_control/find_error_state.h>
			// function for find error in world state
#include	<zeabus_library/zeabus_control/find_robot_error.h>
			// function for find error in frame of robot
#include	<zeabus_library/zeabus_control/find_robot_force.h>
			// function consider about important force
#include	<zeabus_library/zeabus_control/sum_pid_bound_id.h>
			// two line below don't already to choose now
//#include	<zeabus_library/zeabus_control/normal_pid_bound_i.h> // for include pid
//#include	<zeabus_library/zeabus_control/discrete_pid.h> // for include pid type discrete


//------------------> Dynamic Reconfigure We will use in Global variable <-----------------------
double constant_position[4][6] = {	{ 0 , 0 , 0 , 0 , 0 , 0 }
								,	{ 0 , 0 , 0 , 0 , 0 , 0 }
								,	{ 0 , 0 , 0 , 0 , 0 , 0 }
								,	{ 0 , 0 , 0 , 0 , 0 , 0 }};
double constant_velocity[4][6] = {	{ 0 , 0 , 0 , 0 , 0 , 0 }
								,	{ 0 , 0 , 0 , 0 , 0 , 0 }
								,	{ 0 , 0 , 0 , 0 , 0 , 0 }
								,	{ 0 , 0 , 0 , 0 , 0 , 0 }};
double offset_force[6] = { 0 , 0 , 0 , 0 , 0 , 0 };

bool already_loading_constant = false;
bool want_save_constant = false;

void dynamic_reconfigure_callback( zeabus_control::pid_controlConfig &config , uint32_t level){
	printf("Tunning constant value");
	constant_position[0][0] = config.p_x_position;
	constant_position[1][0] = config.i_x_position;
	constant_position[2][0] = config.d_x_position;

	constant_position[0][1] = config.p_y_position;
	constant_position[1][1] = config.i_y_position;
	constant_position[2][1] = config.d_y_position;

	constant_position[0][2] = config.p_z_position;
	constant_position[1][2] = config.i_z_position;
	constant_position[2][2] = config.d_z_position;

	constant_position[0][3] = config.p_roll_position;
	constant_position[1][3] = config.i_roll_position;
	constant_position[2][3] = config.d_roll_position;

	constant_position[0][4] = config.p_pitch_position;
	constant_position[1][4] = config.i_pitch_position;
	constant_position[2][4] = config.d_pitch_position;

	constant_position[0][5] = config.p_yaw_position;
	constant_position[1][5] = config.i_yaw_position;
	constant_position[2][5] = config.d_yaw_position;

	constant_velocity[0][0] = config.p_x_velocity;
	constant_velocity[1][0] = config.i_x_velocity;
	constant_velocity[2][0] = config.d_x_velocity;

	constant_velocity[0][1] = config.p_y_velocity;
	constant_velocity[1][1] = config.i_y_velocity;
	constant_velocity[2][1] = config.d_y_velocity;

	constant_velocity[0][2] = config.p_z_velocity;
	constant_velocity[1][2] = config.i_z_velocity;
	constant_velocity[2][2] = config.d_z_velocity;

	constant_velocity[0][3] = config.p_roll_velocity;
	constant_velocity[1][3] = config.i_roll_velocity;
	constant_velocity[2][3] = config.d_roll_velocity;

	constant_velocity[0][4] = config.p_pitch_velocity;
	constant_velocity[1][4] = config.i_pitch_velocity;
	constant_velocity[2][4] = config.d_pitch_velocity;

	constant_velocity[0][5] = config.p_yaw_velocity;
	constant_velocity[1][5] = config.i_yaw_velocity;
	constant_velocity[2][5] = config.d_yaw_velocity;

	#ifdef _OFFSET_PID__
		offset_force[0] = config.x_offset;
		offset_force[1] = config.y_offset;
		offset_force[2] = config.z_offset;
		offset_force[3] = config.yaw_offset;
		offset_force[4] = config.pitch_offset;
		offset_force[5] = config.yaw_offset;
	#endif
	
	if( already_loading_constant ) want_save_constant = true;
}
