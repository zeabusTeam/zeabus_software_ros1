/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File's name		: pid_control.cpp
//
//	Create on		: Oct 16 , 2018
//	Author			: Supasan Komonlit
//
//	Main purpose	: for using to give detail of pid.h in pid_control class
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include	<zeabus_extension/pid.h>

namespace zeabus_extension{
namespace zeabus_control{

	pid_control::pid_control(double p_constant , double i_constant , double d_constant){
		this->set_constant( p_constant , i_constant , d_constant);
		this->sum_error = 0;
		this->assign_type = false;	
	}

	template<typename value_type> void pid_control::set_type_calculate( bool limit_size 
									, value_type value){
		this->assign_type = true;
		this->limit_size = limit_size;
		if( limit_size ) bound_size = value;
		else bound_value = value;
	}

	template<typename number_type > void pid_control::set_constant( number_type p_constant
						, number_type i_constant , number_type d_constant){
		if( p_constant != -1 ) this->p_constant = p_constant;
		if( i_constant != -1 ) this->i_constant = i_constant;
		if( d_constant != -1 ) this->d_constant = d_constant;
	}

	
}
}
