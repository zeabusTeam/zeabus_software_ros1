////////////////////////////////////// DETAIL ///////////////////////////////////////////////////
//
//	File		: service_number_service.cpp
//	Purpose		: for main_service of control
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Nov 13
//
//	namespace	: zeabus_control
//
///////////////////////////////////// END PART//////////////////////////////////////////////////

#include	<zeabus_library/zeabus_control/service_number_service.h>

bool zeabus_control::number_service::call_set_mode_control(
		zeabus_library::number_service::Request &req
		, zeabus_library::number_service::Response &res
		){
	*mode_control = req.number.data;
	res.ok = true;
	return true;
}
