////////////////////////////////////// DETAIL ///////////////////////////////////////////////////
//
//	File		: find_robot_error_double.cpp 
//	Purpose		: for calculating fine error of robot on robot frame
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Nov 10
//
//	namespace	: zeabus_control
//
///////////////////////////////////// END PART//////////////////////////////////////////////////

#include	<zeabus_library/zeabus_control/find_robot_error_double.h>

namespace zeabus_control{

	void convert_world_to_robot_xy(	double* world_error 
								,	double* robot_error 
								,	double* current_state ){
		double distance_xy = sqrt( pow( world_error[0] , 2) + pow( world_error[1] , 2 ) );
		double distance_yaw = atan2( world_error[1] , world_error[0]);
		double different_yaw;
		find_min_angular( current_state[5] , distance_yaw , different_yaw );
		robot_error[0] = distance_xy * cos( different_yaw );
		robot_error[1] = distance_xy * sin( different_yaw );	
		for( int run = 2 ; run < 6 ; run++ ) robot_error[run] = world_error[run];
	}


	void convert_robot_to_bound_error(	double* robot_error , double* bound_error 
									,	double* ok_error , int size ){
		for( int run = 0 ; run < size ; run++){
			if( fabs( robot_error[run] ) <= ok_error[run] ) bound_error[run] = 0 ;
			else bound_error[run] = robot_error[run];
		}
	}

	void convert_robot_to_bound_error_no_xy( double* robot_error 
											, double* bound_error 
											, double* ok_error 
											, int size ){
		bound_error[0] = 0;
		bound_error[1] = 0;
		for( int run = 2 ; run < size ; run++){
			if( fabs( robot_error[run] ) <= ok_error[run] ) bound_error[run] = 0 ;
			else bound_error[run] = robot_error[run];
		}
	}

}
