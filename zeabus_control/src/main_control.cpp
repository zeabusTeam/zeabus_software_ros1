////////////////////////////////////////// DETAIL ///////////////////////////////////////////////
//
//	File		: main_control.cpp 
//	Purpose		: for call about listen state
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Oct 18
//
//
//
///////////////////////////////////////// END PART //////////////////////////////////////////////

#include	"head_control.cpp"
#include	"other_function.cpp"

int main( int argv , char** argc){

	ros::init( argv , argc , "main_control");

	ros::NodeHandle nh("");

//------------------------------------ FOR PUBLISHER --------------------------------------------
	ros::Publisher tell_force	=	nh.advertise<geometry_msgs::Twist>("/cmd_vel" , 10);
	geometry_msgs::Twist message_force;

//----------------------------------> SET ABOUT STATE <------------------------------------------
	double target_state[6]		=	{ 0 , 0 , 0 , 0 , 0 , 0 };
	double current_state[6]		=	{ 0 , 0 , 0 , 0 , 0 , 0 };
	double current_velocity[6]	=	{ 0 , 0 , 0 , 0 , 0 , 0 };
	zeabus_control::listen_odometry_convert listen_state( current_state , current_velocity );
	ros::Subscriber sub_state = nh.subscribe( "/auv/state" , 1 
								, &zeabus_control::listen_odometry_convert::callback 
								, &listen_state);

//---------------------------------> SET ABOUT VELOCITY <----------------------------------------
	double target_velocity[6]	=	{ 0 , 0 , 0 , 0 , 0 , 0 };
	int use_target_velocity[6]	=	{ 0 , 0 , 0 , 0 , 0 , 0 };
	zeabus_control::listen_twist listen_target_velocity( target_velocity , use_target_velocity );
	ros::Subscriber sub_velocity = nh.subscribe( "zeabus/cmd_vel" , 1 
									, &zeabus_control::listen_twist::callback 
									, &listen_target_velocity);

	double ok_error[6]			=	{ 0.01 , 0.01 , 0.04 , 0.01 , 0.01 , 0.05 };
	double world_error[6]		=	{ 0 , 0 , 0 , 0 , 0 , 0 };
	double robot_error[6]		=	{ 0 , 0 , 0 , 0 , 0 , 0 };
	double bound_error[6]		=	{ 0 , 0 , 0 , 0 , 0 , 0 };
	double pid_force[6]			=	{ 0 , 0 , 0 , 0 , 0 , 0 };
	double robot_force[6]		=	{ 0 , 0 , 0 , 0 , 0 , 0 }; 
	
	int frequency = 30;

//----------------------------------> SET ABOUT SERVICE <----------------------------------------
	zeabus_control::two_point_service service_2_point(	current_state	,	target_state 
													,	robot_error		,	ok_error );
	ros::ServiceServer service_relative_xy = // listen fix position point x y type absolute
		nh.advertiseService(	"/fix_rel_xy" 
								, &zeabus_control::two_point_service::call_relative_xy
								, &service_2_point );
	ros::ServiceServer service_target_xy = // listen fix position point x y type absolute
		nh.advertiseService(	"/fix_abs_xy" 
								, &zeabus_control::two_point_service::call_absolute_xy
								, &service_2_point );

	zeabus_control::one_point_service service_1_point(	current_state	,	target_state
													,	robot_error		,	ok_error );
	ros::ServiceServer	service_target_depth =
		nh.advertiseService(	"/fix_abs_depth"
								, &zeabus_control::one_point_service::call_absolute_depth
								, &service_1_point );
	ros::ServiceServer	service_target_yaw =
		nh.advertiseService(	"/fix_abs_yaw"
								, &zeabus_control::one_point_service::call_absolute_yaw
								, &service_1_point );
	ros::ServiceServer	service_relative_yaw =
		nh.advertiseService(	"/fix_rel_yaw"
								, &zeabus_control::one_point_service::call_relative_yaw
								, &service_1_point );

	zeabus_control::check_state_service service_current_state( current_state	,	target_state
														,	 robot_error	,	ok_error );
	ros::ServiceServer	service_check_state =
		nh.advertiseService(	"/ok_position"
								, &zeabus_control::check_state_service::call_check_state
								, &service_current_state
							);

	zeabus_control::get_target_service service_target_state( current_state	,	target_state
														,	robot_error		,	ok_error );
	ros::ServiceServer sevice_get_state =
		nh.advertiseService(	"/know_target"
								, &zeabus_control::get_target_service::call_get_target
								, &service_target_state);


//------------------------------> SET UP DYNAMIC RECONFIGURE <-----------------------------------
	dynamic_reconfigure::Server<zeabus_control::pid_controlConfig> server;
	dynamic_reconfigure::Server<zeabus_control::pid_controlConfig>::CallbackType function;
	function = boost::bind(&dynamic_reconfigure_callback , _1 , _2); 
	server.setCallback( function );
	zeabus_extension::zeabus_ros::dynamic_reconfigure file_const("zeabus_control" , "constant"
													, "set_01.yaml" , "/main_control");

//--------------------------------> SET UP PID FUNCTION <----------------------------------------
	double bound_value_i[6]		=	{ 1 , 1 , 2 , 1 , 1 , 0.5};
	zeabus_control::normal_pid_bound_i pid_position[6];
	zeabus_control::normal_pid_bound_i pid_velocity[6];
	reset_constant( pid_position , pid_velocity );
	for( int run = 0 ; run < 6 ; run++){
		pid_position[run].limit_value_i_term( bound_value_i[run]);
		pid_position[run].set_frequency( frequency );
		pid_velocity[run].limit_value_i_term( bound_value_i[run]);
		pid_velocity[run].set_frequency( frequency );
	}

	ros::Rate rate( frequency );

/////////////////////////////////////////////////////////////////////////////////////////////////
//===================================== LOOP CONTROL ==========================================//
/////////////////////////////////////////////////////////////////////////////////////////////////

	while( nh.ok() ){
		rate.sleep();
		ros::spinOnce();
//------------------------------> PART ABOUT LOAD OR SAVE CONSTANT <-----------------------------
		if( ! already_loading_constant ){
			file_const.load();
			already_loading_constant = true;
		}
		else if( want_save_constant ){
			file_const.save();
			want_save_constant = false;
		}
		else{}
//---------------------------------------> END PART <--------------------------------------------
		for( int run = 0 ; run < 6 ; run++){
			if( use_target_velocity[run] > 0 ){
				if( run == 1 || run == 0){
					target_state[0] = current_state[0];
					target_state[1] = current_state[1];
				}
				else{
					target_state[run] = current_state[run];
				}
			}
		}
		// find error between current_state with target_state to world_error
		zeabus_control::find_error_position( current_state , target_state , world_error);

		// give world_error to error in robot frame
		zeabus_control::convert_world_to_robot_xy( world_error , robot_error , current_state );

		// fine bound_error by use robot_error and ok_error
		zeabus_control::convert_robot_to_bound_error( robot_error , bound_error , ok_error); 

		// use error of bound_error to calculate force by pid 
		for( int run = 0 ; run < 6 ; run++){
			if( use_target_velocity[run] > 0 ){ // use pid for velocity
				pid_velocity[run].get_result( target_velocity[run] - current_velocity[run] 
											, pid_force[run] );
				pid_position[run].reset_value();
				use_target_velocity[run]--;
			}
			else{ // use pid for position
				pid_position[run].get_result( bound_error[run] , pid_force[run]);
				pid_velocity[run].reset_value();	
			}
		}

		// use pid_force convert to robot_force for send to thruster
		zeabus_control::pid_to_robot_foce_v_1( pid_force , robot_force );

		// publish force to thruster node
		array_to_geometry_twist( robot_force , message_force );	
		tell_force.publish( message_force );	

		// print all data to display	
		system("clear");
		print_all( current_state , target_state , world_error , robot_error , bound_error 
				, pid_force , robot_force 
				, use_target_velocity , current_velocity , target_velocity );
	}
}
