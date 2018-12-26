////////////////////////////////////////// DETAIL ///////////////////////////////////////////////
//
//	File		: normal_pid_bound_i.h 
//	Purpose		: for use pid control in main_control
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Oct 19
//
//	namespace	: zeabus_control
//
///////////////////////////////////////// END PART //////////////////////////////////////////////

#include	<iostream>
#include	<math.h>
#include	<zeabus_library/zeabus_control/normal_pid.h>

#ifndef _ZEABUS_LIBRARY_ZEABUS_CONTROL_NORMAL_PID_BOUND_I_H__
#define _ZEABUS_LIBRARY_ZEABUS_CONTROL_NORMAL_PID_BOUND_I_H__

namespace zeabus_control{

	class normal_pid_bound_i: public normal_pid{
		public:
			normal_pid_bound_i();
			void limit_value_i_term( double value);

		private:
			void individual_calculate( double error , double& result);
			double limit_value;
	};

}

#endif
