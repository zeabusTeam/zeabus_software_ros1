////////////////////////////////////// DETAIL ///////////////////////////////////////////////////
//
//	File		: number_service.h 
//	Purpose		: for main_service of control
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Nov 10
//
//	namespace	: zeabus_control
//
///////////////////////////////////// END PART//////////////////////////////////////////////////

#include	<iostream>
#include	<zeabus_library/number_service.h>
#include	<zeabus_library/zeabus_control/service_control.h>	

#ifndef _ZEABUS_LIBRARY_ZEABUS_CONTROL_SERVICE_NUMBER_SERVICE_H__
#define _ZEABUS_LIBRARY_ZEABUS_CONTROL_SERVICE_NUMBER_SERVICE_H__

namespace zeabus_control{

	class number_service: public main_service{

		public:
			number_service(	double* current_state , double* target_state 
							, double* robot_error , double * ok_error 
							, int& mode_control ) :
								main_service( current_state , target_state 
											, robot_error , ok_error ){
				this->mode_control = mode_control;
			}
			bool call_set_mode_control( zeabus_library::number_service::Request &req
										, zeabus_library::number_service::Response &res );

		private:
			int mode_control;
	};

}	

#endif
