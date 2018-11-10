////////////////////////////////////// DETAIL ///////////////////////////////////////////////////
//
//	File		: service_check_state.cpp 
//	Purpose		: for sub service of control
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Nov 10
//
//	namespace	: zeabus_control
//
///////////////////////////////////// END PART//////////////////////////////////////////////////


#include	<iostream>
#include	<math.h>
#include	<zeabus_library/check_position.h>
#include	<zeabus_library/zeabus_control/service_control.h>

#ifndef _ZEABUS_LIBRARY_CONTROL_ZEABUS_CHECK_STATE_H__
#define _ZEABUS_LIBRARY_CONTROL_ZEABUS_CHECK_STATE_H__

namespace zeabus_control{
	class check_state_service : public main_service{
		public:
			check_state_service ( double* current_state , double* target_state 
					, double* robot_error , double * ok_error ) :
						main_service(	current_state		,	target_state 
									,	robot_error			,	ok_error ){}
			bool call_check_state(	zeabus_library::check_position::Request &req
							,	zeabus_library::check_position::Response &res
							);
	};

}

#endif

#ifdef _CHECK_ERROR_
	#undef _CHECK_ERROR_
#endif
