////////////////////////////////////// DETAIL ///////////////////////////////////////////////////
//
//	File		: find_robot_error.cpp 
//	Purpose		: for calculating fine error of robot on robot frame
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Oct 18
//
//	namespace	: zeabus_control
//
///////////////////////////////////// END PART//////////////////////////////////////////////////

#include	<iostream>
#include	<math.h>
#include	"find_error_state.cpp"

#define _CHECK_ERROR_

#ifndef _find_robot_error_cpp__
#define _find_robot_error_cpp__

namespace zeabus_control{

	template<typename number_type >void convert_world_to_robot_xy(	number_type* world_error 
								,	number_type* robot_error , number_type* current_state ){
		double distance_xy = find_distance( 2 , world_error , robot_error);
		double distance_yaw = atan( world_error[1]/world_error[0]);
		double different_yaw;
		find_min_angular( distance_yaw , current_state[5] , different_yaw );
		#ifdef _CHECK_ERROR_
			std::cout	<< "In covert error to robot frame distance_xy : " << distance_xy 
						<< " <----> distance_yaw : " << distance_yaw << " <-----> different_yaw "
						<< " : " << different_yaw << "\n";
		#endif
		robot_error[0] = distance_xy * cos( different_yaw );
		robot_error[1] = distance_xy * sin( different_yaw );	
		for( int run = 2 ; run < 6 ; run++ ) robot_error[run] = world_error[run];
	}


	template<typename number_type>void convert_robot_to_bound_error( number_type* robot_error 
						, number_type* bound_error , number_type* ok_error , size_t size = 6){
		for( int run = 0 ; run < size ; run++){
			if( fabs( robot_error[run] ) <= ok_error[run] ) bound_error[run] = 0 ;
			else bound_error[run] = robot_error[run];
		}
	}
}

#endif
