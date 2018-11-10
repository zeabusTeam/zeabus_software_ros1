////////////////////////////////////// DETAIL ///////////////////////////////////////////////////
//
//	File		: service_one_point.cpp 
//	Purpose		: for main_service of control
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Oct 18
//
//	namespace	: zeabus_control
//
///////////////////////////////////// END PART//////////////////////////////////////////////////

#include	<iostream>
#include	<zeabus_library/one_point.h>
#include	<zeabus_library/zeabus_control/service_control.h>

#ifndef _ZEABUS_LIBRARY_CONTROL_SERVICE_ONE_POINT_H__
#define _ZEABUS_LIBRARY_CONTROL_SERVICE_ONE_POINT_H__

namespace zeabus_control{

	class one_point_service: public main_service{
		public:
			one_point_service ( double* current_state , double* target_state 
						, double* robot_error , double * ok_error ) :
							main_service( current_state , target_state 
											, robot_error , ok_error ){}
			bool call_absolute_depth( zeabus_library::one_point::Request &req ,
								 zeabus_library::one_point::Response &res );
			bool call_relative_depth( zeabus_library::one_point::Request &req ,
								 zeabus_library::one_point::Response &res );
			bool call_absolute_yaw(	zeabus_library::one_point::Request &req ,
								zeabus_library::one_point::Response &res);
			bool call_relative_yaw(	zeabus_library::one_point::Request &req ,
								zeabus_library::one_point::Response &res);
	};

}

#endif	


