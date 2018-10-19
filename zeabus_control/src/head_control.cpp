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

// include for dynamic reconfigure of ros
#include	<dynamic_reconfigure/server.h>
#include	<zeabus_control/pid_controlConfig.h>

// include zeabus library when sucess to shared
#include	<zeabus_extension/log_data.h>

#include	"listen_twist.cpp"
#include	"listen_odometry.cpp"
#include	"find_error_state.cpp"
#include	"find_robot_error.cpp"
#include	"normal_pid_bound_i.cpp"

double constant_position[3][6] = {	{ 0 , 0 , 0 , 0 , 0 , 0 }
								,	{ 0 , 0 , 0 , 0 , 0 , 0 }
								,	{ 0 , 0 , 0 , 0 , 0 , 0 }};
double constant_velocity[3][6] = {	{ 0 , 0 , 0 , 0 , 0 , 0 }
								,	{ 0 , 0 , 0 , 0 , 0 , 0 }
								,	{ 0 , 0 , 0 , 0 , 0 , 0 }};

void dynamic_reconfigure_callback( zeabus_control::pid_controlConfig &config , uint32_t level){
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

}
