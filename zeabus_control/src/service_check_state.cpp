////////////////////////////////////// DETAIL ///////////////////////////////////////////////////
//
//	File		: service_check_state.cpp 
//	Purpose		: for sub service of control
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Oct 22
//
//	namespace	: zeabus_control
//
///////////////////////////////////// END PART//////////////////////////////////////////////////


#include	<iostream>
#include	"service_control.cpp"
#include	<math.h>
#include	<zeabus_control/check_position.h>
//#define	_CHECK_ERROR_

#ifndef _service_check_state_cpp__
#define _service_check_state_cpp__

namespace zeabus_control{
	class check_state_service : public main_service{
		public:
			check_state_service ( double* current_state , double* target_state 
					, double* robot_error , double * ok_error ) :
						main_service(	current_state		,	target_state 
									,	robot_error			,	ok_error ){}
			bool call_check_state(	check_position::Request &req
							,	check_position::Response &res
							);
	};

		bool check_state_service::call_check_state(	check_position::Request &request
											,	check_position::Response &response ){

			if(request.type.data == "xy"){

				if( abs(robot_error[0]) < ok_error[0] + request.adding 
						&& abs(robot_error[1]) < ok_error[1] + request.adding){
					response.ok = true;
				} else {
					response.ok = false;
				}

			} else if(request.type.data == "z"){

				if( abs(robot_error[2]) < ok_error[2] + request.adding){
					response.ok = true;
				} else{
					response.ok = false;
				}

			} else if(request.type.data == "xyz"){

				if(	abs(robot_error[0]) < ok_error[0] + request.adding
						&& abs(robot_error[1]) < ok_error[1] + request.adding
						&& abs(robot_error[2]) < ok_error[2] + request.adding){
					response.ok = true;
				} else {
					response.ok = false;
				}

			} else if(request.type.data == "yaw"){

				if( abs(robot_error[5]) < ok_error[5] + request.adding){
					response.ok = true;
				}
			} 
		}
}

#endif

#ifdef _CHECK_ERROR_
	#undef _CHECK_ERROR_
#endif
