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

int main( int argv , char** argc){

	ros::init( argv , argc , "main_control");

	ros::NodeHandle nh("");

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
	ros::Subscriber sub_velocity = nh.subscribe( "/cmd/vel" , 1 
									, &zeabus_control::listen_twist::callback 
									, &listen_target_velocity);

	double ok_error[6]			=	{ 0.01 , 0.01 , 0.04 , 0.01 , 0.01 , 0.05 };
	double world_error[6]		=	{ 0 , 0 , 0 , 0 , 0 , 0 };
	double robot_error[6]		=	{ 0 , 0 , 0 , 0 , 0 , 0 };
	double bound_error[6]		=	{ 0 , 0 , 0 , 0 , 0 , 0 };
	double pid_force[6]			=	{ 0 , 0 , 0 , 0 , 0 , 0 };
	double robot_force[6]		=	{ 0 , 0 , 0 , 0 , 0 , 0 }; 
	
	int frequency = 30;

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
	for( int run = 0 ; run < 6 ; run++){
		pid_position[run].set_constant(	constant_position[0][run] 
									,	constant_position[1][run]
									,	constant_position[2][run]);
		pid_velocity[run].set_constant(	constant_velocity[0][run] 
									,	constant_velocity[1][run]
									,	constant_velocity[2][run]);	
		pid_position[run].limit_value_i_term( bound_value_i[run]);
		pid_velocity[run].limit_value_i_term( bound_value_i[run]);
	}

	ros::Rate rate( frequency );

	while( nh.ok() ){
		rate.sleep();
		ros::spinOnce();
		// find error between current_state with target_state to world_error
		zeabus_control::find_error_position( current_state , target_state , world_error);
		// give world_error to error in robot frame
		zeabus_control::convert_world_to_robot_xy( world_error , robot_error , current_state );
		// use error of robot to calculate force by pid 
		
	};			
}
