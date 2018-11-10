////////////////////////////////////// DETAIL ///////////////////////////////////////////////////
//
//	File		: find_robot_force.h
//	Purpose		: for can make parity of force to do that
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Nov 10
//
//	namespace	: zeabus_control
//
///////////////////////////////////// END PART//////////////////////////////////////////////////

#include	<iostream>
#include	<math.h>


#ifndef _ZEABUS_LIBRARY_ZEABUS_CONTROL_FIND_ROBOT_FORCE_H__
#define _ZEABUS_LIBRARY_ZEABUS_CONTROL_FIND_ROBOT_FORCE_H__

namespace zeabus_control{

	// version 1 will target on x and y
	template<typename number_type> void pid_to_robot_foce_v_1( number_type* pid_force 
															,  number_type* robot_force);

	// version 2 will add force for maintain depth of robot
	template<typename number_type> void pid_to_robot_foce_v_2( number_type* pid_force 
															,  number_type* robot_force);
}

#include	"./../../../src/zeabus_control/find_robot_force.cpp"

#endif
