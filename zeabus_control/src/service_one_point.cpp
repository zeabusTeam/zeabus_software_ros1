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
#include	<zeabus_control/one_point.h>
#include	"service_control.cpp"
//#define _CHECK_ERROR_

#ifndef _service_one_point_cpp__
#define _service_one_point_cpp__

namespace zeabus_control{

	class one_point_service: public main_service{
		public:
			one_point_service ( double* current_state , double* target_state 
						, double* robot_error , double * ok_error ) :
							main_service( current_state , target_state 
											, robot_error , ok_error ){}
			bool call_absolute_depth( one_point::Request &req ,
								 one_point::Response &res );
			bool call_relative_depth( one_point::Request &req ,
								 one_point::Response &res );
			bool call_absolute_yaw(	zeabus_control::one_point::Request &req ,
								one_point::Response &res);
			bool call_relative_yaw(	zeabus_control::one_point::Request &req ,
								one_point::Response &res);
	};

	bool one_point_service::call_absolute_depth( one_point::Request &req ,
								 one_point::Response &res ){
		this->target_state[2] = req.point_1;
		res.success = true;
		return true;
	}
	bool one_point_service::call_relative_depth( one_point::Response &req ,
								one_point::Response &req){
		this->target_state[2] += req.point_1;
		res.success = true;
		return true;
	}
	bool one_point_service::call_absolute_yaw(	one_point::Request &req ,
								one_point::Response &res){
		this->target_state[5] = req.point_1;
		res.success = true;
		return true;
	}
	bool one_point_service::call_relative_yaw(	one_point::Request &req ,
								one_point::Response &res){
		this->target_state[5] += req.point_1;
		res.success = true;
		return true;

	}

}

#endif	


