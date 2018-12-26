////////////////////////////////////////// DETAIL ///////////////////////////////////////////////
//
//	File		: discrete_pid.h 
//	Purpose		: for use pid control in main_control
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Nov 2
//
//	namespace	: zeabus_control
//
///////////////////////////////////////// END PART //////////////////////////////////////////////

#include	<iostream>
#include	<math.h>
#include	<vector>
#include	<zeabus_library/zeabus_control/normal_pid.h>

#ifndef _ZEABUS_LIBRARY_ZEABUS_CONTROL_DISCRETE_PID_H__
#define _ZEABUS_LIBRARY_ZEABUS_CONTROL_DISCRETE_PID_H__

namespace zeabus_control{

	class discrete_pid: public normal_pid{
		public:
			discrete_pid();
			void set_sum_term( bool use_sum_term , double bound_sum_value);
			void reset_value();

		private:
			void individual_calculate( double error , double& result);
			bool use_sum_term;
			double bound_sum_value;
			std::vector< double > result;
	};

}

#endif
