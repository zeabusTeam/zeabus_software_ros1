////////////////////////////////////// DETAIL ///////////////////////////////////////////////////
//
//	File		: find_error_state.h 
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

#ifndef _ZEABUS_LIBRARY_ZEABUS_CONTROL_FIND_ERROR_STATE_H__
#define _ZEABUS_LIBRARY_ZEABUS_CONTROL_FIND_ERROR_STATE_H__

namespace zeabus_control{

	template<typename number_type> void convert_angular( number_type& point);

	template<typename number_type> number_type find_distance( int size
															, number_type* point_1
															, number_type* point_2 );

	template<typename number_type> void find_min_angular( number_type& start 
												, number_type& target , number_type& result);

	template<typename number_type> void find_error_position( number_type *point_1 
					, number_type *point_2 , number_type *point_result );
}
	#include	"./../../../src/zeabus_control/find_error_state.cpp"

#endif	

