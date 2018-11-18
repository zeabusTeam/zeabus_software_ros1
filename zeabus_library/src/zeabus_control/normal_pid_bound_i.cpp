////////////////////////////////////////// DETAIL ///////////////////////////////////////////////
//
//	File		: normal_pid_bound_i.cpp 
//	Purpose		: for use pid control in main_control
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Oct 19
//
//	namespace	: zeabus_control
//
///////////////////////////////////////// END PART //////////////////////////////////////////////

#include	<zeabus_library/zeabus_control/normal_pid_bound_i.h>

namespace zeabus_control{

	normal_pid_bound_i::normal_pid_bound_i() : normal_pid(){}

	void normal_pid_bound_i::limit_value_i_term( double value){
		this->limit_value = value;
	}

	void normal_pid_bound_i::individual_calculate( double error , double& result){
		this->result_p_term = ( this->p_constant * error );
		this->result_i_term += this->i_constant * error * this->period_time;
		if( fabs( this->result_i_term) > this->limit_value ){
			this->result_i_term = copysign( this->limit_value , this->result_i_term);
		}
		this->result_d_term = ( error - this->list_error[0] ) / 
								this->period_time * this->d_constant ;
		result = this->result_p_term + this->result_i_term + this->result_d_term ;
	}	

}
