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
	double current_state[10]	=	{ 0 , 0 , 0 , 0 , 0 , 0 };
	double current_velocity[10]	=	{ 0 , 0 , 0 , 0 , 0 , 0 };
	zeabus_control::listen_odometry_convert listen_state( current_state , current_velocity );
	ros::Subscriber sub_state = nh.subscribe( "/auv/state" , 1 
								, &zeabus_control::listen_odometry_convert::callback 
								, &listen_state);

//---------------------------------> SET ABOUT VELOCITY <----------------------------------------
	double target_state[10]		=	{ 0 , 0 , 0 , 0 , 0 , 0 };
	int use_target_velocity[10]	=	{ 0 , 0 , 0 , 0 , 0 , 0 };
	zeabus_control::listen_twist listen_target_velocity( target_state , use_target_velocity );
	ros::Subscriber sub_velocity = nh.subscribe( "/cmd/vel" , 1 
									, &zeabus_control::listen_twist::callback 
									, &listen_target_velocity);
		
			
}
