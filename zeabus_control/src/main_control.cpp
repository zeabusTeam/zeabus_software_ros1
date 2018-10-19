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

	double world_error[6]		=	{ 0 , 0 , 0 , 0 , 0 , 0 };
	double robot_error[6]		=	{ 0 , 0 , 0 , 0 , 0 , 0 };
	double pid_force[6]			=	{ 0 , 0 , 0 , 0 , 0 , 0 };
	double robot_force[6]		=	{ 0 , 0 , 0 , 0 , 0 , 0 }; 
	
	int frequency = 30;

//------------------------------> SET UP DYNAMIC RECONFIGURE <-----------------------------------
	dynamic_reconfigure::Server<zeabus_control::pid_controlConfig> server;
	dynamic_reconfigure::Server<zeabus_control::pid_controlConfig>::CallbackType function;
	function = boost::bind(&dynamic_reconfigure_callback , _1 , _2); 
	server.setCallback( function );


//--------------------------------> SET UP PID FUNCTION <----------------------------------------
	zeabus_control::normal_pid_bound_i pid_position[6];
	zeabus_control::normal_pid_bound_i pid_velocity[6];

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
