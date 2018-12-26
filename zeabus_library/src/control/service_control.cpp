////////////////////////////////////// DETAIL ///////////////////////////////////////////////////
//
//	File		: service_control.cpp 
//	Purpose		: for main_service of control
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Oct 18
//
//	namespace	: zeabus_control
//
///////////////////////////////////// END PART//////////////////////////////////////////////////

#include	<zeabus_library/zeabus_control/service_control.h>

zeabus_control::main_service::main_service( double* current_state , double* target_state
								, double* robot_error , double* ok_error ){
	this->current_state = current_state;
	this->target_state = target_state;
	this->robot_error = robot_error;
	this->ok_error = ok_error;
}
