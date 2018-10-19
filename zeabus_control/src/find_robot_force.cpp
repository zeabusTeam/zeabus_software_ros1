////////////////////////////////////// DETAIL ///////////////////////////////////////////////////
//
//	File		: find_robot_force.cpp 
//	Purpose		: for can make parity of force to do that
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Oct 20
//
//	namespace	: zeabus_control
//
///////////////////////////////////// END PART//////////////////////////////////////////////////

#include	<iostream>
#include	<math.h>

#ifndef _find_robot_force_cpp__
#define _find_robot_force_cpp__

namespace zeabus_control{
	template<typename number_type> void pid_to_robot_foce_v_1( number_type* pid_force 
															,  number_type* robot_force){
		// version 1 will target on x and y
		for( int run = 3 ; run < 6 ; run++) robot_force[run] = pid_force[run];

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
		
	}
}

#endif
