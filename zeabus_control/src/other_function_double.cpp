////////////////////////////////////////// DETAIL ///////////////////////////////////////////////
//
//	File		: other_function_double.cpp 
//	Purpose		: for collecting other function from refactor
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Nov 12
//
//
//
///////////////////////////////////////// END PART //////////////////////////////////////////////

#include	<zeabus_control/head_library_control.h>

void array_to_geometry_twist( double* array_set 
							, geometry_msgs::Twist& data){
	data.linear.x	= array_set[0];
	data.linear.y	= array_set[1];
	data.linear.z	= array_set[2];
	data.angular.x	= array_set[3];
	data.angular.y	= array_set[4];
	data.angular.z	= array_set[5];
}

void array_to_point3_msg( double* array_set
						, zeabus_library::Point3& data){
	data.x = array_set[0];
	data.y = array_set[1];
	data.z = array_set[2];
}

void array_to_type2_msg( double* array_set
										, zeabus_library::Type2& data){
	double linear_set[3]=  { array_set[0] , array_set[1] , array_set[2]};
	array_to_point3_msg( linear_set , data.linear );
	double angular_set[3] = { array_set[3] , array_set[4] , array_set[5]};
	array_to_point3_msg( angular_set , data.angular );
}

void array_to_state_msg( double* state_set
						, double* velocity_set
						, zeabus_library::State& data){
	data.header.stamp = ros::Time::now();
	array_to_type2_msg( state_set , data.position );
	array_to_type2_msg( velocity_set , data.velocity );
}
