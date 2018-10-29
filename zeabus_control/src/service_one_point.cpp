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
#include	<zeabus_control/one_point.srv>

//#define _CHECK_ERROR_

#ifndef _service_one_point_cpp__
#define _service_one_point_cpp__

namespace zeabus_control{

	class one_point_service: public main_service{
		public:
			one_point_service ( double* current_state , double* target_state 
						, double* robot_error , double * ok_error ) :
							main_service( double* current_state , double* target_state 
											, double* robot_error , double * ok_error ){}
			bool call_abs_depth( zeabus_control::one_point::Request &req ,
								 zeabus_control::one_point::Response &res );
			bool call_abs_yaw(	zeabus_control::one_point::Request &req ,
								zeabus_control::one_point::Request &res);
			bool call_rel_yaw(	zeabus_control::one_point::Request &req ,
								zeabus_control::one_point::Request &res);
	};

	bool one_point_service::call_abs_depth( zeabus_control::one_point::Request &req ,
								 zeabus_control::one_point::Response &res ){
		this->target_state[2] = req.point_1;
		res.success = true;
		return true;
	}
	bool one_point_service::call_abs_yaw(	zeabus_control::one_point::Request &req ,
								zeabus_control::one_point::Request &res){
		this->target_state[5] = req.point_1;
		res.success = true;
		return true;
	}
	bool one_point_service::call_rel_yaw(	zeabus_control::one_point::Request &req ,
								zeabus_control::one_point::Request &res){
		this->target_state[5] += req.point_1;
		res.success = true;
		return true;

	}

}

#endif	


