////////////////////////////////////// DETAIL ///////////////////////////////////////////////////
//
//	File		: find_robot_error.h
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
#include	<zeabus_library/zeabus_control/find_error_state.h>

#ifndef _ZEABUS_LIBRARY_ZEABUS_CONTROL_FIND_ROBOT_ERROR_H__
#define _ZEABUS_LIBRARY_ZEABUS_CONTROL_FIND_ROBOT_ERROR_H__

namespace zeabus_control{

	template<typename number_type >void convert_world_to_robot_xy(	number_type* world_error 
								,	number_type* robot_error , number_type* current_state );


	template<typename number_type>void convert_robot_to_bound_error( number_type* robot_error 
						, number_type* bound_error , number_type* ok_error , int size = 6);

	template<typename number_type>void convert_robot_to_bound_error_no_xy( 
									number_type* robot_error 
									, number_type* bound_error 
									, number_type* ok_error , int size = 6);

}
	#include	"./../../../src/zeabus_control/find_robot_error.cpp"

#endif

