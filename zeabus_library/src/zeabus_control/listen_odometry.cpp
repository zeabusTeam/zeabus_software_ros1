////////////////////////////////////// DETAIL ///////////////////////////////////////////////////
//
//	File		: listen_odometry.cpp 
//	Purpose		: for call about listen state
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Nov 10
//
//	namespace	: zeabus_control
//
///////////////////////////////////// END PART//////////////////////////////////////////////////

#include	<zeabus_library/zeabus_control/listen_odometry.h>

namespace zeabus_control{

	template<typename number_type >void nav_msgs_to_array( const nav_msgs::Odometry& data 
							, number_type* state , number_type* velocity){
			state[0] = data.pose.pose.position.x;
			state[1] = data.pose.pose.position.y;
			state[2] = data.pose.pose.position.z;
			state[3] = data.pose.pose.orientation.x;
			state[4] = data.pose.pose.orientation.y;
			state[5] = data.pose.pose.orientation.z;
			state[6] = data.pose.pose.orientation.w;
			velocity[0] = data.twist.twist.linear.x;
			velocity[1] = data.twist.twist.linear.y;
			velocity[2] = data.twist.twist.linear.z;
			velocity[3] = data.twist.twist.angular.x;
			velocity[4] = data.twist.twist.angular.y;
			velocity[5] = data.twist.twist.angular.z;
	}

	listen_odometry::listen_odometry( double* state , double* velocity){
		this->state = state;
		this->velocity = velocity;
		#ifdef _CHECK_ERROR_
		std::cout	<< "pointer of data : number <----> " << this->state 
					<< " : " << state << "\n";
		std::cout	<< "pointer of data : number <----> " << this->velocity 
					<< " : " << velocity << "\n";
		#endif	
	}

	void listen_odometry::callback( const nav_msgs::Odometry& message){
		nav_msgs_to_array( message , this->state , this->velocity);	
	}

	listen_odometry_convert::listen_odometry_convert( double* state 
													, double* velocity
													, double* target){
		this->first_time = true;
		this->state = state;
		this->velocity = velocity;
		this->target = target;
		#ifdef _CHECK_ERROR_
		std::cout	<< "pointer of data : number <----> " << this->state 
					<< " : " << state << "\n";
		std::cout	<< "pointer of data : number <----> " << this->velocity 
					<< " : " << velocity << "\n";
		#endif	
	}

	void listen_odometry_convert::callback( const nav_msgs::Odometry& message){
		tf::Quaternion quaternion( message.pose.pose.orientation.x,
									message.pose.pose.orientation.y,
									message.pose.pose.orientation.z,
									message.pose.pose.orientation.w			
								);
		tfScalar roll, pitch, yaw;
		tf::Matrix3x3(quaternion).getRPY( roll, pitch, yaw);
		this->state[0] = message.pose.pose.position.x;
		this->state[1] = message.pose.pose.position.y;
		this->state[2] = message.pose.pose.position.z;
		this->state[3] = roll;
		this->state[4] = pitch;
		this->state[5] = yaw;
		this->velocity[0] = message.twist.twist.linear.x;
		this->velocity[1] = message.twist.twist.linear.y;
		this->velocity[2] = message.twist.twist.linear.z;
		this->velocity[3] = message.twist.twist.angular.x;
		this->velocity[4] = message.twist.twist.angular.y;
		this->velocity[5] = message.twist.twist.angular.z;
		if( this->first_time ){
			this->target[0] = this->state[0];
			this->target[1] = this->state[1];
			this->target[2] = this->state[2];
			this->target[3] = 0;
			this->target[4] = 0;
			this->target[5] = this->state[5];
			this->first_time = false;
		}	
	}

}

