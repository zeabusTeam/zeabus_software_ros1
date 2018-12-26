////////////////////////////////////////// DETAIL ///////////////////////////////////////////////
//
//	File		: normal_pid.cpp 
//	Purpose		: for use pid control
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Oct 19
//
//	namespace	: zeabus_control
//
///////////////////////////////////////// END PART //////////////////////////////////////////////

#include	<zeabus_library/zeabus_control/normal_pid.h>

namespace zeabus_control{


	normal_pid::normal_pid(){
		for( ; this->list_error.size() < 2; ) this->list_error.push_back( 0 );
	}
	
	void normal_pid::set_constant(	double constant_01 , double constant_02 
								,	double constant_03 , double constant_04){
		this->p_constant = constant_01;
		this->i_constant = constant_02;
		this->d_constant = constant_03;
		this->offset_constant = constant_04;
	}

	void normal_pid::set_frequency( double frequency){
		this->period_time = 1/frequency;
	}

	void normal_pid::reset_value(){
		this->result_p_term = 0 ;
		this->result_i_term = 0 ;
		this->result_d_term = 0 ;
	}

	void normal_pid::get_result( double error , double& result){
		this->individual_calculate( error , result );
	}
	
	void normal_pid::individual_calculate( double error ,double & result){}
}
