////////////////////////////////////////// DETAIL ///////////////////////////////////////////////
//
//	File		: sum_pid_bound_id.cpp 
//	Purpose		: for use pid control and use sum term is Intergral and Differential
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Oct 19
//
//	namespace	: zeabus_control
//
///////////////////////////////////////// END PART //////////////////////////////////////////////

#include	<zeabus_library/zeabus_control/sum_pid_bound_id.h>

namespace zeabus_control{

	sum_pid_bound_id::sum_pid_bound_id(){
		for( ; this->list_error.size() < 2; ) this->list_error.push_back( 0 );
	}

	void sum_pid_bound_id::set_constant(	double constant_01 , double constant_02 
										,	double constant_03 ){
		this->p_constant = constant_01;
		this->i_constant = constant_02;
		this->d_constant = constant_03;
	}

	void sum_pid_bound_id::limit_value_sum_term(  double value){
		this->limit_value = value;
	}

	void sum_pid_bound_id::set_frequency( double frequency){
		this->period_time = 1/frequency;
	}

	void sum_pid_bound_id::individual_calculate( double error , double& result ){
		printf("error is %.4f\t" , error );
		this->list_error[0] = this->list_error[1];
		this->list_error[1] = error;
		this->result_p_term = this->list_error[1] * this->p_constant;
		this->result_id_term += ( this->list_error[1] + this->list_error[0] ) / 2 
									* this->i_constant * this->period_time 
							+ ( this->list_error[1] - this->list_error[2] ) / this->period_time
									* this->d_constant;
		if( fabs( this->result_id_term ) > this->limit_value ){
			this->result_id_term = copysign( this->limit_value , this->result_id_term  );
		}

		result = this->result_p_term + this->result_id_term ;
		printf("result is %.4f\t%.4f\t%.4f\n" , this->result_p_term 
											, this->result_id_term 
											, result);
	}

	void sum_pid_bound_id::reset_value(){
		this->result_id_term = 0;
		for( int run = 0 ; run < this->list_error.size() ; run++ ){
			this->list_error[run] = 0;
		}
	}

	void sum_pid_bound_id::get_result( double error , double& result){
		this->individual_calculate( error , result );
	}

}
