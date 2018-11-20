////////////////////////////////////////// DETAIL ///////////////////////////////////////////////
//
//	File		: head_control_double.cpp 
//	Purpose		: include all file to collect variable class and all function
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Nov 12
//
//
//
///////////////////////////////////////// END PART //////////////////////////////////////////////

#include	<iostream> // standard of cpp
#include	<ros/ros.h> // standard of ros system
#include	<cstdlib>

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
#include	<zeabus_library/zeabus_control/find_error_state_double.h>
			// function for find error in world state
#include	<zeabus_library/zeabus_control/find_robot_error_double.h>
			// function for find error in frame of robot
#include	<zeabus_library/zeabus_control/find_robot_force_double.h>
			// function consider about important force
#include	<zeabus_library/zeabus_control/sum_pid_bound_id.h>
			// two line below don't already to choose now
//#include	<zeabus_library/zeabus_control/normal_pid_bound_i.h> // for include pid
//#include	<zeabus_library/zeabus_control/discrete_pid.h> // for include pid type discrete


void array_to_geometry_twist( double* array_set 
							, geometry_msgs::Twist& data
							);

void array_to_point3_msg( double* array_set
						, zeabus_library::Point3& data
						);

void array_to_type2_msg( double* array_set
						, zeabus_library::Type2& data
						);

void array_to_state_msg( double* state_set
						, double* velocity_set
						, zeabus_library::State& data
						);
