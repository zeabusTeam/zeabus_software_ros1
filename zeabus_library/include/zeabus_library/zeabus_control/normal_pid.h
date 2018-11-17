////////////////////////////////////////// DETAIL ///////////////////////////////////////////////
//
//	File		: normal_pid.h 
//	Purpose		: for use pid control
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Oct 19
//
//	namespace	: zeabus_control
//
///////////////////////////////////////// END PART //////////////////////////////////////////////

#include	<iostream>
#include	<math.h>
#include	<vector>

#ifndef _ZEABUS_LIBRARY_ZEABUS_CONTROL_NORMAL_PID_H__
#define _ZEABUS_LIBRARY_ZEABUS_CONTROL_NORMAL_PID_H__

namespace zeabus_control{

	class normal_pid{

		public:
			normal_pid(); //  init object
			virtual void set_constant(	double constant_01 = 0
									,	double constant_02 = 0
									,	double constant_03 = 0
									,	double constant_04 = 0);
			void get_result( double error , double& result); // use pass by reference
			virtual void reset_value(); // reset all term
			void set_frequency( double frequency);
			
		protected:
			double p_constant;
			double i_constant;
			double d_constant;
			double offset_constant;
			double result_p_term;
			double result_i_term;
			double result_d_term;
			std::vector<double> list_error;
			double period_time;
			
		private:
			virtual void individual_calculate( double error , double& result);
	};

}

#endif
