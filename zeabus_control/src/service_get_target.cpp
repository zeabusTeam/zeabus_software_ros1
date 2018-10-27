////////////////////////////////////// DETAIL ///////////////////////////////////////////////////
//
//	File		: service_get_target.cpp 
//	Purpose		: for sub service of control
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Oct 21
//
//	namespace	: zeabus_control
//
///////////////////////////////////// END PART//////////////////////////////////////////////////

#include	<iostream>
#include	<zeabus_control/get_target.h>

#ifndef _service_get_target_cpp__
#define _service_get_target_cpp__

namespace zeabus_control{

	class get_target_service : public main_service{

		public:
			get_target_service ( double* current_state , double* target_state 
						, double* robot_error , double * ok_error ) :
							main_service( double* current_state , double* target_state 
											, double* robot_error , double * ok_error ){}
			bool call_get_target( zeabus_control::get_target::Request &req ,
								 zeabus_control::get_target::Response &res );
	};

	bool get_target_service::call_get_target(	zeabus_control::get_target::Request &req ,
												zeabus_control::get_target::Response &res){
		if( req.type.data == "xy"){
			res.target_01 = target_state[0];
			res.target_02 = target_state[1];
		}
		else if( req.type.data == "yaw"){
			res.target_01 = target_state[5];
		}
		else if( req.type.data == "z"){
			res.target_01 = target_state[2];
		}
		else{
			std::cout << "Don't have this type In target_service\n"; 
		}
	}

}

#endif
