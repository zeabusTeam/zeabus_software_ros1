/////////////////////////////////////// DEATAIL ////////////////////////////////////////////////
//	FILE		: find_error.h
//	SOURCE		: find_error_state.cpp find_robot_error.cpp
//	Purpose		: For calculating aboutn error of position
//	
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Oct 20
//
//	namespace	: zeabus_control
////////////////////////////////////// END PART /////////////////////////////////////////////////

#include	<iostream>
#include	<math.h>

#ifndef _find_error_h__
#define _find_error_h__

namespace zeabus_control{
	//-------> SORCE : find_error_state.cpp 
	template<typename number_type> void convert_angular( number_type& point);
	template<typename number_type> number_type find_distance( int size 
															, number_type* point_1 
															, number_type* point_2 );
	template<typename number_type> void find_min_angular( number_type& start
											, number_type& target , number_type& result);
	template<typename number_type> void find_error_position( number_type* point_1 
											, number_type* point_2 , number_type* point_result);

	//-------> SORCE : find_robot_error.cpp
	template<typename number_type> void convert_world_to_robot_xy( number_type* world_error
								, number_type* robot_error , number_type* current_state );
	template<typename number_type> void convert_robot_to_bound_error( number_type* robot_error 
											, number_type* bound_error , number_type* ok_error 
											, int size = 6);
}

#endif
