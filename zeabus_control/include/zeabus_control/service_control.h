////////////////////////////////////// DETAIL ///////////////////////////////////////////////////
//
//	File		: service_control.h
//	Purpose		: header of main service in control part
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Nov 09
//
//	namespace	: zeabus_control
//
///////////////////////////////////// END PART//////////////////////////////////////////////////

#include	<iostream>
#ifndef _ZEABUS_CONTROL_SERVICE_CONTROL_H__
#define _ZEABUS_CONTROL_SERVICE_CONTROL_H__

namespace zeabus_control{

	class main_service{
		public: 
			main_service( double* current_state , double* target_state 
						, double* robot_error , double * ok_error );

		protected:
			double* current_state;
			double* target_state;
			double* robot_error;
			double* ok_error;
	};
}

#endif
