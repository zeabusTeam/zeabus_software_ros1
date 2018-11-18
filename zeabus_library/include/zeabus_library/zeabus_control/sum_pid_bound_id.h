////////////////////////////////////////// DETAIL ///////////////////////////////////////////////
//
//	File		: sum_pid_bound_id.h 
//	Purpose		: for use pid control and use sum term is Intergral and Differential
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Oct 19
//
//	namespace	: zeabus_control
//
///////////////////////////////////////// END PART //////////////////////////////////////////////

#include	<stdio.h>
#include	<iostream>
#include	<vector>
#include	<math.h>

#ifndef _ZEABUS_LIBRARY_ZEABUS_CONTROL_SUM_PID_BOUND_ID_H__
#define _ZEABUS_LIBRARY_ZEABUS_CONTROL_SUM_PID_BOUND_ID_H__

namespace zeabus_control{

	class sum_pid_bound_id{

		public:
			sum_pid_bound_id();	
			void limit_value_sum_term( double value );
			void reset_value();
			void set_constant(	double constant_01 = 0
							,	double constant_02 = 0
							,	double constant_03 = 0
			);
			void set_frequency( double frequency);
			void get_result( double error , double& result); // use pass by reference

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
			void individual_calculate( double error , double& result);
			double limit_value;
			double result_id_term;
	};

}

#endif
