/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File's name		: discrete_pid.cpp
//
//	Create on		: Oct 16 , 2018
//	Author			: Supasan Komonlit
//
//	Main purpose	: for using to give detail of pid.h in discrete_pid class
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include	<zeabus_extension/pid.h>

namespace zeabus_extension{
namespace zeabus_control{

	discrete_pid::discrete_pid( double p_constant , double i_constant , double d_constant)
			: pid_control( p_constant , i_constant , d_constant ){
		this->pid_result.resize( 2 , 0 );	
		this->list_error.resize( 3 , 0 );
	} 
	
	void discrete_pid::calculate_result( double error , double& result){
		this->list_error[2] = error;
		result += this->p_constant*( this->list_error[2] + this->list_error[1] )
				+ this->i_constant*( this->list_error[2])
				+ this->d_constant*( this->list_error[2] - this->list_error[1]*2 
									+ this->list_error[0]);
		this->list_error[0] = this->list_error[1];
		this->list_error[1] = this->list_error[2];	
	}

	void discrete_pid::reset_value(){
		if( limit_size ){
			this->list_error.resize( this->bound_size , 0);	
		}
		else{
			this->list_error[0] = 0;
			this->list_error[1] = 0;
			this->list_error[2] = 0;
		}
	}

	void bound_value_integral(){
	}

	void bound_size_integral(){

	}

}
}
