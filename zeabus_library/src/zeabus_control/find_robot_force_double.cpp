////////////////////////////////////// DETAIL ///////////////////////////////////////////////////
//
//	File		: find_robot_force_double.cpp 
//	Purpose		: for can make parity of force to do that
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Nov 10
//
//	namespace	: zeabus_control
//
///////////////////////////////////// END PART//////////////////////////////////////////////////

#include	<zeabus_library/zeabus_control/find_robot_force_double.h>

namespace zeabus_control{

	// version 1 will target on x and y
	void pid_to_robot_force_v_1( double* pid_force ,  double* robot_force){

		for( int run = 2 ; run < 6 ; run++) robot_force[run] = pid_force[run];

		if( fabs( pid_force[0] - pid_force[1] ) > 1.5 ){
			if( fabs( pid_force[0] ) > fabs( pid_force[1] ) ){
				robot_force[1] = 0;
				robot_force[0] = pid_force[0];
			}
			else{
				robot_force[0] = 0;
				robot_force[1] = pid_force[1];
			}
		}
		else{
			robot_force[0] = pid_force[0];
			robot_force[1] = pid_force[1];
		}
		
	}

	// version 2 will add force for maintain depth of robot and filter force for roll pitch out
	void pid_to_robot_force_v_2( double* pid_force ,  double* robot_force , double* bound_force){

		for( int run = 0 ; run < 6 ; run++){
			if( fabs( pid_force[run] ) > bound_force[run] )
				robot_force[run] = copysign( bound_force[run] , pid_force[run]);
			else
				robot_force[run] = pid_force[run];
		}
		robot_force[3] = 0;
		robot_force[4] = 0;
		robot_force[2] += -1;
	}	

	// version 3 will add force for maintain depth of robot and filter force only pitch out
	void pid_to_robot_force_v_3( double* pid_force , double* robot_force , double* bound_force){
		for( int run = 0 ; run < 6 ; run++ ){
			if( fabs( pid_force[run] ) > bound_force[run] )
				robot_force[run] = copysign( bound_force[run] , pid_force[run] );
			else
				robot_force[run] = pid_force[run];
		}

		robot_force[2] += -1;
		robot_force[4] *= -1;
	}

	void convert_pid_to_robot_force(	double* pid_force		, double* robot_force
									,	double* bound_min_force , double* bound_max_force
									,	bool* fix_force_bool	, double* fix_force_value ){
		for( int run = 0 ; run < 6 ; run++ ){
			if( fix_force_bool[ run ] ){
				if( fabs( pid_force[run]) >= fix_force_value[ run ] ){
					robot_force[ run ] = copysign( fix_force_value[run] , pid_force[run] );
				}
			}
			else{
				if( pid_force[run] < bound_min_force[run] ) 
					robot_force[ run ] = bound_min_force[ run ];
				else if( pid_force[run] > bound_max_force[run] )
					robot_force[ run ] = bound_max_force[ run ];
				else
					robot_force[ run ] = pid_force[ run ];
					
			}
		}
	}
}

