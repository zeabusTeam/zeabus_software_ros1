////////////////////////////////////// DETAIL ///////////////////////////////////////////////////
//
//	File		: find_error_state_double.h 
//	Purpose		: for calculating error state of robot
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Nov 10
//
//	namespace	: zeabus_control
//
///////////////////////////////////// END PART//////////////////////////////////////////////////

#include	<iostream>
#include	<math.h>
#define PI 3.1415926535897

#ifndef _ZEABUS_LIBRARY_ZEABUS_CONTROL_FIND_ERROR_STATE_DOUBLE_H__
#define _ZEABUS_LIBRARY_ZEABUS_CONTROL_FIND_ERROR_STATE_DOUBLE_H__

namespace zeabus_control{

	void convert_angular( double& point);

	double find_distance(	int size
							, double* point_1
							, double* point_2 );

	void find_min_angular(	double& start 
							, double& target 
							, double& result );

	void find_error_position(	double *point_1 
								, double *point_2 
								, double *point_result );

	void find_error_position_non_xy(	double *point_1 
										, double *point_2
										, double *point_result );

	void find_error_position_inverse_y(	double *point_1 
										, double *point_2
										, double *point_result );
}

#endif	

