#include	"head_control.h"

#define	pid_test
#define test_

void init(){
	// allocate area in heap function for pid in 2 case
	pid_position = (zeabus_extension::zeabus_control::discrete_pid*)std::malloc(
						6*sizeof(zeabus_extension::zeabus_control::discrete_pid)
					);				
	pid_velocity = (zeabus_extension::zeabus_control::discrete_pid*)std::malloc(
						6*sizeof(zeabus_extension::zeabus_control::discrete_pid)
					);
	// set up type of value
	for( int run = 0 ; run < 6 ; run++){
		pid_position[run].set_type_calculate( false , 3);
		pid_position[run].set_constant(	constant_position[run][0] 
								,	constant_position[run][1]
								,	constant_position[run][2]);

		pid_velocity[run].set_type_calculate( false , 3);
		pid_velocity[run].set_constant(	constant_position[run][0] 
								,	constant_position[run][1]
								,	constant_position[run][2]);
	}				
}

int main( int argc , char** argv){
	// init name of this node when run by use ros run
	ros::init( argc , argv , "main_control"); 

	ros::NodeHandle nh; // handle for use in this code


	ros::Rate rate(30); // set frequency for run
	while( nh.ok() ){ // run until ros is stop
		
		
		ros::spinOnce(); // open time for listening subscriber or service
		rate.sleep(); // sleep in one period of rate frequency
	}
}
