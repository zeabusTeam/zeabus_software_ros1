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

#include	<iostream>

#define _CHECK_ERROR_

#ifndef _service_control_cpp__
#define _service_control_cpp__

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

	main_service::main_service( double* current_state , double* target_state
								, double* robot_error , double* ok_error ){
		this->current_state = current_state;
		this->target_state = target_state;
		this->robot_error = robot_error;
		this->ok_error = ok_error;
	}

}

#endif
