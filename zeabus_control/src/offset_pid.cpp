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
#include	"normal_pid.cpp"

#define _CHECK_ERROR_

#ifndef _offset_pid_cpp__
#define _offset_pid_cpp__

namespace zeabus_control{

	class offset_pid: public normal_pid{
		public:
			offset_pid();
			void set_offset( double offset_value);

		private:
			void individual_calculate( double error , double& result);
			double offset_value;
	};

	offset_pid::offset_pid() : normal_pid(){}

	void offset_pid::set_offset( double offset_value ){
		this->offset_value = offset_value;
	}

	void offset_pid::individual_calculate( double error , double& result ){
		this->list_error[1] = error;
		this->result_p_term = this->list_error[1] * this->p_constant;
		this->result_i_term = (this->list_error[0] + this->list_error[1]) / 2 
							  * this->period_time * this->i_constant;
		this->result_d_term = (this->list_error[1] - this->list_error[0]) 
							  * this->d_constant / this->period_time;
		result = this->offset_value + this->result_p_term 
				+this->result_i_term + this->result_d_term ;
	}

}

#endif

#ifdef _CHECK_ERROR_
	#undef _CHECK_ERROR_
#endif
