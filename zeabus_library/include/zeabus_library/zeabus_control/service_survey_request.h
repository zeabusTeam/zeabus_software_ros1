////////////////////////////////////// DETAIL ///////////////////////////////////////////////////
//
//	File		: service_survey_request.h 
//	Purpose		: for sub service of control
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Nov 18
//
//	namespace	: zeabus_control
//
///////////////////////////////////// END PART//////////////////////////////////////////////////

#include	<iostream>
#include	<math.h>
#include	<zeabus_library/SurveyRequest.h>
#include	<zeabus_library/zeabus_control/service_control.h>

#ifndef _ZEABUS_LIBRARY_CONTROL_ZEABUS_SURVEY_REQUEST_H__
#define _ZEABUS_LIBRARY_CONTROL_ZEABUS_SURVEY_REQUEST_H__

namespace zeabus_control{
	class survey_service : public main_service{

		public:
			survey_service(	double* current_state		, double* target_state
							, double* robot_error		, double* ok_error
							, bool* fix_force_bool		, double* fix_force_value );
			bool call_survey_request(	zeabus_library::SurveyRequest::Request &req
									,	zeabus_library::SurveyRequest::Response &res );

		private:
			bool fix_force_bool[6];
			double fix_force_value[6];	
		
	};
}

#endif
