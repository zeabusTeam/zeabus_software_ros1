////////////////////////////////////// DETAIL ///////////////////////////////////////////////////
//
//	File		: find_error_state_double.cpp 
//	Purpose		: for calculating error state of robot
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Nov 10
//
//	namespace	: zeabus_control
//
///////////////////////////////////// END PART//////////////////////////////////////////////////

#include	<zeabus_library/zeabus_control/find_error_state_double.h>

namespace zeabus_control{

	void convert_angular( double& point){
		for( ; point < 0 || point > 2*PI ; ){
			if( point < 0 ) point += 2*PI;
			else if( point > 2*PI ) point -= 2*PI;
			else{
				std::cout	<< "<--ZEABUS_EXTENSION--> Error in loop function"
							<< " convert_angular in file find_error_position.cpp\n";
			}
		}
	}

	double find_distance( int size , double* point_1 , double* point_2 ){
		double result = 0;
		for( int run = 0 ; run < size ; run++){
			result += pow( point_1[run] - point_2[run] , 2);
		}
		return sqrt( result );
	}

	void find_min_angular( double& start , double& target , double& result){
		convert_angular( start );
		convert_angular( target );
		double result_01 = target - start;
		double result_02;
		if( result_01 < 0 )  result_02 = PI*2 + ( result_01 );
		else  result_02 = -1 * ( PI*2 - ( result_01 ) );
		if( fabs( result_01 ) < fabs( result_02 ) ) result = result_01;
		else result = result_02;
	}

	// this is find distance when roll and pitch = 0 radian
	void find_error_position( double *point_1 , double *point_2 
							, double *point_result ){
		point_result[0] = point_2[0] - point_1[0];	
		point_result[1] = point_2[1] - point_1[1];	
		point_result[2] = point_2[2] - point_1[2];
		find_min_angular( point_1[3] , point_2[3] , point_result[3]);	
		find_min_angular( point_1[4] , point_2[4] , point_result[4]);	
		find_min_angular( point_1[5] , point_2[5] , point_result[5]);	
	}

	void find_error_position_non_xy(	double *point_1	, double *point_2 
										, double *point_result ){
		point_2[0] = point_1[0];
		point_2[1] = point_1[1];
		find_error_position( point_1 , point_2 , point_result );
	}

	void find_error_position_inverse_y(	double *point_1
										, double *point_2
										, double *point_result ){
		find_error_position_non_xy( point_1 , point_2 , point_result );
		point_result[5] *= -1;
	}
}

