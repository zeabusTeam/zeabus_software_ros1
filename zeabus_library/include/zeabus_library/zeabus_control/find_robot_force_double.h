////////////////////////////////////// DETAIL ///////////////////////////////////////////////////
//
//	File		: find_robot_force_double.h
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
	void pid_to_robot_force_v_1(	double* pid_force , double* robot_force);

	// version 2 will add force for maintain depth of robot and filter force for roll pitch out
	void pid_to_robot_force_v_2( double* pid_force , double* robot_force , double* bound_force);

	// version 3 will add force for maintain depth of robot and filter force only pitch out
	void pid_to_robot_force_v_3( double* pid_force , double* robot_force , double* bound_force);

}

#endif
