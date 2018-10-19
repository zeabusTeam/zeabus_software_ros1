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

#include	<iostream>
#include	<math.h>
#include	<vector>

#define _CHECK_ERROR_

#ifndef _normal_pid_cpp__
#define _normal_pid_cpp__

namespace zeabus_control{

	class normal_pid{

		public:
			normal_pid(); //  init object
			void set_constant( double p_constant , double i_constant , double d_constant );
			void get_result( double& result); // use pass by reference
			void reset_value(); // reset all term
			void set_frequency( double frequency);
			
		protected:
			double p_constant;
			double i_constant;
			double d_constant;
			double result_p_term;
			double result_i_term;
			double result_d_term;
			std::vector<double> list_error;
			double period_time;
			
		private:
			void individual_calculate( double& result);
	};

	normal_pid::normal_pid(){
		for( ; this->list_error.size() < 2; ) this->list_error.push_back( 0 );
	}
	
	void normal_pid::set_constant( double p_constant , double i_constant , double d_constant){
		this->p_constant = p_constant;
		this->i_constant = i_constant;
		this->d_constant = d_constant;
	}

	void normal_pid::set_frequency( double frequency){
		this->period_time = 1/frequency;
	}

	void normal_pid::reset_value(){
		this->result_p_term = 0 ;
		this->result_i_term = 0 ;
		this->result_d_term = 0 ;
	}

	void normal_pid::get_result( double& result){
		this->individual_calculate( result );
	}
	
	void normal_pid::individual_calculate( double & result){
		#ifdef _CHECK_ERROR_
			std::cout << "This is individual_calculate in class normal_pid\n";
		#endif
	}
}

#endif

#ifdef _CHECK_ERROR_
	#undef _CHECK_ERROR_
#endif
