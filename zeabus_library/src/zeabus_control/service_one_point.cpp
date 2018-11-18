////////////////////////////////////// DETAIL ///////////////////////////////////////////////////
//
//	File		: service_one_point.cpp 
//	Purpose		: for main_service of control
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Nov 10
//
//	namespace	: zeabus_control
//
///////////////////////////////////// END PART//////////////////////////////////////////////////

#include	<zeabus_library/zeabus_control/service_one_point.h>


namespace zeabus_control{

	bool one_point_service::call_absolute_depth( zeabus_library::OnePoint::Request &req ,
								 zeabus_library::OnePoint::Response &res ){
		this->target_state[2] = req.point_1;
		res.success = true;
		return true;
	}
	bool one_point_service::call_relative_depth( zeabus_library::OnePoint::Request &req ,
								zeabus_library::OnePoint::Response &res){
		this->target_state[2] += req.point_1;
		res.success = true;
		return true;
	}
	bool one_point_service::call_absolute_yaw(	zeabus_library::OnePoint::Request &req ,
								zeabus_library::OnePoint::Response &res){
		this->target_state[5] = req.point_1;
		res.success = true;
		return true;
	}
	bool one_point_service::call_relative_yaw(	zeabus_library::OnePoint::Request &req ,
								zeabus_library::OnePoint::Response &res){
		this->target_state[5] += req.point_1;
		res.success = true;
		return true;

	}

}
