////////////////////////////////////// DETAIL ///////////////////////////////////////////////////
//
//	File		: listen_odometry.h 
//	Purpose		: for call about listen state
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Nov 10
//
//	namespace	: zeabus_control
//
///////////////////////////////////// END PART//////////////////////////////////////////////////

#include	<iostream>
#include	<nav_msgs/Odometry.h>
#include 	<tf/transform_datatypes.h>
#include	<tf/transform_listener.h>

#ifndef _ZEABUS_LIBRARY_ZEABUS_CONTROL_LISTEN_ODOMETRE_H__
#define _ZEABUS_LIBRARY_ZEABUS_CONTROL_LISTEN_ODOMETRE_H__

namespace zeabus_control{

	class listen_odometry{
		public:
			void callback( const nav_msgs::Odometry& message);
			listen_odometry( double* state , double* velocity);

		private:
			double *state;
			double *velocity;
			double *target;
	};

	class listen_odometry_convert{
		public:
			void callback( const nav_msgs::Odometry& message);
			listen_odometry_convert( double* state , double* velocity , double* target);

		private:
			bool first_time;
			double *state;
			double *velocity;
			double *target;
	};

}

#endif
