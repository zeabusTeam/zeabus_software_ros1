////////////////////////////////////// DETAIL ///////////////////////////////////////////////////
//
//	File		: service_two_point.cpp 
//	Purpose		: for main_service of control
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Nov 10
//
//	namespace	: zeabus_control
//
///////////////////////////////////// END PART//////////////////////////////////////////////////

#include	<zeabus_library/zeabus_control/service_two_point.h>

namespace zeabus_control{

	bool two_point_service::call_relative_xy( zeabus_library::TwoPoint::Request &req ,
								 zeabus_library::TwoPoint::Response &res ){
			this->target_state[0] += req.point_1 * cos( target_state[5]);
			this->target_state[1] += req.point_1 * sin( target_state[5]);
			this->target_state[0] += req.point_2 * cos( target_state[5] + PI/2);
			this->target_state[1] += req.point_2 * sin( target_state[5] + PI/2);
			res.success = true;
			return true;
	}

	bool two_point_service::call_absolute_xy( zeabus_library::TwoPoint::Request &req ,
								zeabus_library::TwoPoint::Response &res ){
			this->target_state[0] = req.point_1;
			this->target_state[1] = req.point_2;
			res.success = true;
			return true;
	}

}
