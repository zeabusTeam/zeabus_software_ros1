/*
	File name			:	SubOdometry.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , FEB 06
	Date last modified	:	2018 , ??? ??
	Purpose				:	

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Using & Maintain
							
	Namespace			:	zeabus_library/subscriber
*/
//====================>

#include	<stdio.h>

#include	<iostream>

#include	<ros/ros.h>

#include	<nav_msgs/Odometry.h>

#include	<geometry_msgs/TwistStamped.h>

#ifndef _ZEABUS_LIBRARY_SUBSCRIBER_SEBODOMETRY_H__
#define _ZEABUS_LIBRARY_SUBSCRIBER_SEBODOMETRY_H__

namespace zeabus_library{

namespace subscriber{

	class SubOdometry{

		public:	
			SubOdometry( nav_msgs::Odometry* data );

			void register_data( nav_msgs::Odometry* data );

			virtual void callback( const nav_msgs::Odometry& message );

			void callback_ttl( const nav_msgs::Odometry& message );
	
			virtual void register_ttl( int* ttl , int number = 20 );

			void set_ttl( int number );

		protected:
			int constant_ttl;

			int* ttl; // time to live

		private:
			nav_msgs::Odometry* data;

	};

	class SubOdometryByTwistStamped : public SubOdometry{
	
		public:
			SubOdometryByTwistStamped( geometry_msgs::TwistStamped* data );
			
			void register_data( geometry_msgs::TwistStamped* data );

			void callback( const nav_msgs::Odometry& message );	
			void callback_ttl( const nav_msgs::Odometry& message );

		private:
			geometry_msgs::TwistStamped* data;	

	};

}

}

#endif
