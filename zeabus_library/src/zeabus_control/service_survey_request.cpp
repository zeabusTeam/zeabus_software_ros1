////////////////////////////////////// DETAIL ///////////////////////////////////////////////////
//
//	File		: service_survey_request.cpp 
//	Purpose		: for sub service of control
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Nov 18
//
//	namespace	: zeabus_control
//
///////////////////////////////////// END PART//////////////////////////////////////////////////

#include	<zeabus_library/zeabus_control/service_survey_request.h>

namespace zeabus_control{

	survey_service::survey_service(	double* current_state		, double* target_state
									, double* robot_error		, double* ok_error
									, bool* fix_force_bool		, double* fix_force_value ):
			main_service( current_state , target_state , robot_error , ok_error ){
		this->fix_force_bool = fix_force_bool;
		this->fix_force_value = fix_force_value;
	}

	bool survey_service::call_survey_request(	zeabus_library::SurveyRequest::Request& req
												, zeabus_library::SurveyRequest::Response& res ){
		if( req.axis.data == "x"){
			this->fix_force_bool[ 0 ] = true;
			this->fix_force_value[ 0 ] = req.max_force; 
			this->target_state[0] += req.distance * cos( target_state[5]);
			this->target_state[1] += req.distance * sin( target_state[5]);
		}
		else if( req.axis.data == "y"){
			this->fix_force_bool[ 1 ] = true;
			this->fix_force_value[ 1 ] = req.max_force; 
			this->target_state[0] += req.distance * cos( target_state[5] + PI/2);
			this->target_state[1] += req.distance * sin( target_state[5] + PI/2);
		}
	}
	
}
