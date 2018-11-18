////////////////////////////////////// DETAIL ///////////////////////////////////////////////////
//
//	File		: service_check_state.cpp 
//	Purpose		: for sub service of control
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Nov 10
//
//	namespace	: zeabus_control
//
///////////////////////////////////// END PART//////////////////////////////////////////////////


#include	<zeabus_library/zeabus_control/service_check_state.h>

namespace zeabus_control{

	bool check_state_service::call_check_state(	zeabus_library::CheckPosition::Request &request
										,	zeabus_library::CheckPosition::Response &response ){
		response.ok = false;
		if(request.type.data == "xy"){
			if( fabs(robot_error[0]) < ok_error[0] + request.adding 
					&& fabs(robot_error[1]) < ok_error[1] + request.adding){
				response.ok = true;
			}
		} 

		else if(request.type.data == "z"){
			if( fabs(robot_error[2]) < ok_error[2] + request.adding){
				response.ok = true;
			}
		} 

		else if(request.type.data == "xyz"){
			if(	fabs(robot_error[0]) < ok_error[0] + request.adding
					&& fabs(robot_error[1]) < ok_error[1] + request.adding
					&& fabs(robot_error[2]) < ok_error[2] + request.adding){
				response.ok = true;
			} 
		} 
		else if(request.type.data == "yaw"){
			if( fabs(robot_error[5]) < ok_error[5] + request.adding){
				response.ok = true;
			}
		}
		return true; 
	}

}
