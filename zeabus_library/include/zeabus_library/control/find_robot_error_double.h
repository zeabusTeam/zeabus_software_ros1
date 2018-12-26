////////////////////////////////////// DETAIL ///////////////////////////////////////////////////
//
//	File		: find_robot_error_double.h
//	Purpose		: for calculating fine error of robot on robot frame
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Nov 10
//
//	namespace	: zeabus_control
//
///////////////////////////////////// END PART//////////////////////////////////////////////////

#include	<iostream>
#include	<math.h>
#include	<zeabus_library/zeabus_control/find_error_state_double.h>

#ifndef _ZEABUS_LIBRARY_ZEABUS_CONTROL_FIND_ROBOT_ERROR_DOUBLE_H__
#define _ZEABUS_LIBRARY_ZEABUS_CONTROL_FIND_ROBOT_ERROR_DOUBLE_H__

namespace zeabus_control{

	void convert_world_to_robot_xy(	double* world_error 
									, double* robot_error , double* current_state );


	void convert_robot_to_bound_error( double* robot_error 
									, double* bound_error 
									, double* ok_error 
									, int size = 6);

	void convert_robot_to_bound_error_no_xy(	double* robot_error 
									, double* bound_error 
									, double* ok_error 
									, int size = 6);

}

#endif

