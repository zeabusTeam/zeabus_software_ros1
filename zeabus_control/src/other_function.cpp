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

//#define _CHECK_ERROR_

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
	#ifdef _OFFSET_PID__
		for( int run = 0 ; run < 6 ; run++){
			pid_position[run].set_offset( )
		}
	#endif
}

template<typename number_type> void array_to_geometry_twist( number_type* array_set 
										, geometry_msgs::Twist& data){
	data.linear.x	= array_set[0];
	data.linear.y	= array_set[1];
	data.linear.z	= array_set[2];
	data.angular.x	= array_set[3];
	data.angular.y	= array_set[4];
	data.angular.z	= array_set[5];
}

template<typename number_type> void array_to_point3_msg( number_type* array_set
										, zeabus_library::Point3& data){
	data.x = array_set[0];
	data.y = array_set[1];
	data.z = array_set[2];
}

template<typename number_type> void array_to_type2_msg( number_type* array_set
										, zeabus_library::Type2& data){
	double linear_set[3]=  { array_set[0] , array_set[1] , array_set[2]};
	array_to_point3_msg( linear_set , data.linear );
	double angular_set[3] = { array_set[3] , array_set[4] , array_set[5]};
	array_to_point3_msg( angular_set , data.angular );
}

template<typename number_type> void array_to_state_msg( number_type* state_set
										, number_type* velocity_set
										, zeabus_library::State& data){
	data.header.stamp = ros::Time::now();
	array_to_type2_msg( state_set , data.position );
	array_to_type2_msg( velocity_set , data.velocity );
}



template<typename number_type , typename count_type> void print_all( 
				  number_type* current_state	, number_type* target_state 
				, number_type* world_error		, number_type* robot_error
				, number_type* bound_error		, number_type* pid_force 
				, number_type* robot_force		, count_type* use_target_velocity		
				, number_type* current_velocity , number_type* target_velocity
				, number_type* offset_force = 0){
	printf("Information        :%8s%8s%8s%8s%8s%8s\n\n","x","y","z","roll","pitch","yaw");
	printf("current_state      :%8.3lf%8.3lf%8.3lf%8.3lf%8.3lf%8.3lf\n\n" ,
				current_state[0] , current_state[1] , current_state[2]
			,	current_state[3] , current_state[4] , current_state[5] );	
	printf("target_state       :%8.3lf%8.3lf%8.3lf%8.3lf%8.3lf%8.3lf\n\n" ,
				target_state[0] , target_state[1] , target_state[2]
			,	target_state[3] , target_state[4] , target_state[5] );	
	printf("world_error        :%8.3lf%8.3lf%8.3lf%8.3lf%8.3lf%8.3lf\n\n" ,
				world_error[0] , world_error[1] , world_error[2]
			,	world_error[3] , world_error[4] , world_error[5] );	
	printf("robot_error        :%8.3lf%8.3lf%8.3lf%8.3lf%8.3lf%8.3lf\n\n" ,
				robot_error[0] , robot_error[1] , robot_error[2]
			,	robot_error[3] , robot_error[4] , robot_error[5] );	
	printf("bound_error        :%8.3lf%8.3lf%8.3lf%8.3lf%8.3lf%8.3lf\n\n" ,
				bound_error[0] , bound_error[1] , bound_error[2]
			,	bound_error[3] , bound_error[4] , bound_error[5] );	
	if( &offset_force == 0){
		printf("offset_force       :%8.3lf%8.3lf%8.3lf%8.3lf%8.3lf%8.3lf\n\n" ,
					offset_force[0] , offset_force[1] , offset_force[2]
				,	offset_force[3] , offset_force[4] , offset_force[5] );	
	}
	printf("pid_force          :%8.3lf%8.3lf%8.3lf%8.3lf%8.3lf%8.3lf\n\n" ,
				pid_force[0] , pid_force[1] , pid_force[2]
			,	pid_force[3] , pid_force[4] , pid_force[5] );	
	printf("robot_force        :%8.3lf%8.3lf%8.3lf%8.3lf%8.3lf%8.3lf\n\n" ,
				robot_force[0] , robot_force[1] , robot_force[2]
			,	robot_force[3] , robot_force[4] , robot_force[5] );	
	printf("current_velocity   :%8.3lf%8.3lf%8.3lf%8.3lf%8.3lf%8.3lf\n\n" ,
				current_velocity[0] , current_velocity[1] , current_velocity[2]
			,	current_velocity[3] , current_velocity[4] , current_velocity[5] );	
	printf("target_velocity    :%8.3lf%8.3lf%8.3lf%8.3lf%8.3lf%8.3lf\n\n" ,
				target_velocity[0] , target_velocity[1] , target_velocity[2]
			,	target_velocity[3] , target_velocity[4] , target_velocity[5] );	
	printf("use_target_velocity:%8d%8d%8d%8d%8d%8d\n\n" ,
				use_target_velocity[0] , use_target_velocity[1] , use_target_velocity[2]
			,	use_target_velocity[3] , use_target_velocity[4] , use_target_velocity[5] );	
}

#ifdef _CHECK_ERROR_
	#undef _CHECK_ERROR_
#endif
