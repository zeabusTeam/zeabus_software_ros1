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

#include	<iostream>
#include	<math.h>
#include	"normal_pid.cpp"

#define _CHECK_ERROR_

#ifndef _normal_pid_bound_i_cpp__
#define _normal_pid_bound_i_cpp__

namespace zeabus_control{

	class normal_pid_bound_i: public normal_pid{
		public:
			normal_pid_bound_i();
			void limit_value_i_term( double value);

		private:
			void individual_calculate( double error , double& result);
			double limit_value;
	};

	normal_pid_bound_i::normal_pid_bound_i() : normal_pid(){}

	void normal_pid_bound_i::limit_value_i_term( double value){
		this->limit_value = value;
	}

	void normal_pid_bound_i::individual_calculate( double error , double& result){
		#ifdef _CHECK_ERROR_
			std::cout	<< "this individual_calculate in normal_pid_bound_i "
						<< " p : i : d --> " << this->p_constant << " : " 
						<< this->i_constant << " : " << this->d_constant << "\n";
		#endif
		this->result_p_term = ( this->p_constant * error );
		this->result_i_term += this->i_constant * error * this->period_time;
		if( fabs( this->result_i_term) > limit_value ){
			this->result_i_term = copysign( limit_value , this->result_i_term);
		}
		this->result_d_term = ( error - this->list_error[0] ) / 
								this->period_time * this->d_constant ;
		result = this->result_p_term + this->result_i_term + this->result_d_term ;
	}	

}

#endif


#ifdef _CHECK_ERROR_
	#undef _CHECK_ERROR_
#endif
