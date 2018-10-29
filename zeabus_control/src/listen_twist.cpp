////////////////////////////////////// DETAIL ///////////////////////////////////////////////////
//
//	File		: listen_twist.cpp 
//	Purpose		: for callback about listen velocity
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Oct 18
//
//	namespace	: zeabus_control
//
///////////////////////////////////// END PART//////////////////////////////////////////////////

#include	<iostream>
#include	<geometry_msgs/Twist.h>
#define _CHECK_ERROR_

#define epsilon 2.22e-16
#ifndef _listen_twist_cpp__
#define _listen_twist_cpp__

namespace zeabus_control{
	
	class listen_twist{
		public:
			void callback( const geometry_msgs::Twist& message);
			listen_twist( double* velocity );
			listen_twist( double* velocity , int* set_use_velocity);
		private:
			double* velocity;
			// for specific purpose
			int* set_use_velocity;
			int set_value;
			bool use_velocity;
	};

	void listen_twist::callback( const geometry_msgs::Twist& message){
		this->velocity[0] = message.linear.x;
		this->velocity[1] = message.linear.y;
		this->velocity[2] = message.linear.z;
		this->velocity[3] = message.angular.x;
		this->velocity[4] = message.angular.y;
		this->velocity[5] = message.angular.z;
		if( this->use_velocity ){
			for( int run = 0 ; run < 6 ; run++){
				if( fabs(this->velocity[run]) > epsilon ){
					this->set_use_velocity[run] = this->set_value;
				}
				else this->set_use_velocity[run] = 0;
			}
		}
	}

	listen_twist::listen_twist( double* velocity , int* set_use_velocity ){
		this->velocity = velocity;
		this->set_use_velocity = set_use_velocity;
		this->use_velocity = true;
		this->set_value = 10;
		#ifdef _CHECK_ERROR_
			std::cout	<< "listen velocity pointer class : local --->" << this->velocity 
						<< " : " << velocity << "\n";
		#endif
	}

	listen_twist::listen_twist( double* velocity){
		this->velocity = velocity;
		this->use_velocity = false;
		#ifdef _CHECK_ERROR_
			std::cout	<< "listen velocity pointer class : local --->" << this->velocity 
						<< " : " << velocity << "\n";
		#endif
	}
  
}

#endif

#ifdef _CHECK_ERROR_
	#undef _CHECK_ERROR_
#endif
