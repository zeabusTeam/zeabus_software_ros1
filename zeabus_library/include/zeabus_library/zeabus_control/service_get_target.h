////////////////////////////////////// DETAIL ///////////////////////////////////////////////////
//
//	File		: service_get_target.cpp 
//	Purpose		: for sub service of control
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Nov 10
//
//	namespace	: zeabus_control
//
///////////////////////////////////// END PART//////////////////////////////////////////////////

#include	<iostream>
#include	<zeabus_library/get_target.h>
#include	<zeabus_library/zeabus_control/service_control.h>

#ifndef _ZEABUS_LIBRARY_CONTROL_SERVICE_GET_TARGET_H__
#define _ZEABUS_LIBRARY_CONTROL_SERVICE_GET_TARGET_H__

namespace zeabus_control{

	class get_target_service : public main_service{

		public:
			get_target_service ( double* current_state , double* target_state 
						, double* robot_error , double * ok_error ) :
							main_service( current_state , target_state 
											, robot_error , ok_error ){}
			bool call_get_target( zeabus_library::get_target::Request &req ,
								  zeabus_library::get_target::Response &res );
	};

}

#endif
