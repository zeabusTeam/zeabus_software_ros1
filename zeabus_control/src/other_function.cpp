////////////////////////////////////////// DETAIL ///////////////////////////////////////////////
//
//	File		: other_function.cpp 
//	Purpose		: for collecting other function from refactor
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Oct 20
//
//
//
///////////////////////////////////////// END PART //////////////////////////////////////////////

template<typename pid_type>void reset_constant( pid_type* pid_position , pid_type* pid_velocity){
	for( int run = 0 ; run < 6 ; run++){
		pid_position[run].set_constant(	constant_position[0][run] 
									,	constant_position[1][run]
									,	constant_position[2][run]);
		pid_velocity[run].set_constant(	constant_velocity[0][run] 
									,	constant_velocity[1][run]
									,	constant_velocity[2][run]);
		pid_position[run].reset_value();	
		pid_velocity[run].reset_value();	
	}
}
