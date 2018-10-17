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



	dynamic_reconfigure = new zeabus_extension::zeabus_ros::dynamic_reconfigure(
								package_dynamic_reconfigure , path_file_dynamic_reconfigure 
							,	name_file_dynamic_reconfigure , node_name_dynamic_reconfigure);
	dynamic_reconfigure->load();

	ros::Rate rate(30); // set frequency for run

	while( nh.ok() ){ // run until ros is stop
		rate.sleep(); // sleep in one period of rate frequency
		ros::spinOnce(); // open time for listening subscriber or service
		zeabus_extension::zeabus_control::find_error_position(	current_state , target_state 
																, different_state);
		divide_2_dimension_xy( different_state , robot_error , current_state , bound_error);	
		for( int run = 0 ; run < 6 ; run++){
			if( use_velocity[run] == 0 ){
				pid_position[run].calculate_result( robot_error[run] , pid_force[run]);
				pid_velocity[run].reset_value();
			}
			else{
				use_velocity[run] --;
				pid_velocity[run].calculate_result( target_velocity[run] - current_velocity[run]
													, pid_force[run]);
				pid_position[run].reset_value();
			}
		}
			
	}
}
