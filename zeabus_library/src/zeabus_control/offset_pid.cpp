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

#include	<zeabus_library/zeabus_control/offset_pid.h>

namespace zeabus_control{

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
