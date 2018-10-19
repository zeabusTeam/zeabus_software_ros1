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


