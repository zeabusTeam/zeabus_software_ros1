////////////////////////////////////// DETAIL ///////////////////////////////////////////////////
//
//	File		: service_get_target.cpp 
//	Purpose		: for sub service of control
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Nov 10
//
//	namespace	: zeabus_control
//
///////////////////////////////////// END PART//////////////////////////////////////////////////

#include	<zeabus_library/zeabus_control/service_get_target.h>

namespace zeabus_control{

	bool get_target_service::call_get_target(	zeabus_library::get_target::Request &req ,
												zeabus_library::get_target::Response &res){
		if( req.type.data == "xy"){
			res.target_01 = target_state[0];
			res.target_02 = target_state[1];
		}
		else if( req.type.data == "yaw"){
			res.target_01 = target_state[5];
			res.target_02 = 0;
		}
		else if( req.type.data == "z"){
			res.target_01 = target_state[2];
			res.target_02 = 0;
		}
		else{
			std::cout << "Don't have this type In target_service\n"; 
		}
	}
}

