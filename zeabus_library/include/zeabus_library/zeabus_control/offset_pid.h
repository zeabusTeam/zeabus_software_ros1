////////////////////////////////////////// DETAIL ///////////////////////////////////////////////
//
//	File		: offset_pid.cpp 
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
#include	<zeabus_library/zeabus_control/normal_pid.h>


#ifndef _ZEABUS_LIBRARY_ZEABUS_CONTROL_OFFSET_PID_H__
#define _ZEABUS_LIBRARY_ZEABUS_CONTROL_OFFSET_PID_H__

namespace zeabus_control{

	class offset_pid: public normal_pid{
		public:
			offset_pid();
			void set_offset( double offset_value);

		private:
			void individual_calculate( double error , double& result);
			double offset_value;
	};

}

#endif
