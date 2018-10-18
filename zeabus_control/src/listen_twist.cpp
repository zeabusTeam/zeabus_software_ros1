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

#ifndef _listen_twist_cpp__
#define _listen_twist_cpp__

namespace zeabus_control{
	
	template < class array_type > class listen_velocity{
		public:
			void callback( const geometry_msgs::Twist& message);
			listen_velocity( array_type* velocity );
			listen_velocity( array_type* velocity , int* set_use_velocity);
		private:
			array_type* velocity;
			// for specific purpose
			int* set_use_velocity;
			bool use_velocity;
	};

	template <class array_type> void listen_velocity::callback(
					const geometry_msgs::Twist& message){
		this->velocity[0] = message.linear.x;
		this->velocity[1] = message.linear.y;
		this->velocity[2] = message.linear.z;
		this->velocity[3] = message.angular.x;
		this->velocity[4] = message.angular.y;
		this->velocity[5] = message.angular.z;
	}

	template < class array_type > listen_velocity::listen_velocity( array_type* velocity 
					, int* set_use_velocity ){
		this->velocity = velocity;
		this->set_use_velocity = set_use_velocity;
		this->use_velocity = true;
		#ifdef _CHECK_ERROR_
			std::cout	<< "listen velocity pointer class : local --->" << this->velocity 
						<< " : " << velocity << "\n";
		#endif
	}

	template < class array_type > listen_velocity::listen_velocity( array_type* velocity){
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
