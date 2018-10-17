void divide_2_dimension_xy(	double *world_error , double* robot_error 
						,	double* robot_state , double* bound_error){
	double distance_xy = sqrt( pow( world_error[0] , 2) + pow( world_error[1] , 2) );
	double distance_yaw = atan( world_error[1] / world_error[0]);
	double different_yaw = robot_state[5] - world_error[5];
	robot_error[0] = distance_xy * cos( different_yaw );
	robot_error[1] = distance_xy * sin( different_yaw );
	robot_error[2] = world_error[2];	
	robot_error[3] = world_error[3];	
	robot_error[4] = world_error[4];	
	robot_error[5] = world_error[5];	
	decision_error( robot_error , bound_error );	
}
