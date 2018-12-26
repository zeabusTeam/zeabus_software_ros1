////////////////////////////////////// DETAIL ///////////////////////////////////////////////////
//
//	File		: listen_twist.h 
//	Purpose		: for callback about listen velocity
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Nov 10
//
//	namespace	: zeabus_control
//
///////////////////////////////////// END PART//////////////////////////////////////////////////

#include	<iostream>
#include	<geometry_msgs/Twist.h>

#define epsilon 2.22e-16
#ifndef _ZEABUS_LIBRARY_ZEABUS_CONTROL_LISTEN_TWIST_H__
#define _ZEABUS_LIBRARY_ZEABUS_CONTROL_LISTEN_TWIST_H__

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

}

#endif
