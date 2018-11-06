////////////////////////////////////////// DETAIL ///////////////////////////////////////////////
//
//	File		: discrete_pid.cpp 
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
#include	"normal_pid.cpp"

#define _CHECK_ERROR_

#ifndef _discrete_pid_cpp__
#define _discrete_pid_cpp__

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

	discrete_pid::discrete_pid() : normal_pid(){
		for( ; this->list_error.size() < 3 ; ) this->list_error.push_back(0);
		for( ; this->result.size() < 2 ; ) this->result.push_back(0);
	}

	void discrete_pid::set_sum_term( bool use_sum_term , double bound_sum_value ){
		this->bound_sum_value = bound_sum_value;
		this->use_sum_term = use_sum_term;
	}

	void discrete_pid::reset_value(){
		this->result[0] = 0;
		this->result[1] = 0;
		this->list_error[0] = 0;
		this->list_error[1] = 0;
		this->list_error[2] = 0;
	}

	void discrete_pid::individual_calculate( double error , double& result ){
		this->list_error[2] = error;
		this->result_p_term = ( this->list_error[2] - this->list_error[1] )*this->p_constant;
		this->result_i_term = this->list_error[2]*this->i_constant;
		this->result_d_term = (this->list_error[2] - 2*this->list_error[1] + this->list_error[0])
							* this->d_constant;
		if( use_sum_term){
			if( fabs( this->result[0]) > this->bound_sum_value ){
				this->result[0] = this->result[0]/fabs( this->result[0]) 
								* this->bound_sum_value;	
			}
			this->result[1] = this->result[0] 
							+ this->result_p_term
							+ this->result_i_term
							+ this->result_d_term;
			this->result[0] = this->result[1];
		}
		else{
			this->result[1] = this->list_error[2] * this->p_constant;
							+ this->result_i_term
							+ this->result_d_term;
		}
		this->list_error[0] = this->list_error[1];
		this->list_error[1] = this->list_error[2];
		result = this->result[1];
	}

}

#endif


#ifdef _CHECK_ERROR_
	#undef _CHECK_ERROR_
#endif
