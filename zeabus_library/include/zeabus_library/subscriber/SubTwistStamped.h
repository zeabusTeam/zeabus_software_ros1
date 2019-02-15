/*
	File name			:	SubTwistStamped.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , FEB 09
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

#include	<math.h>

#include	<iostream>

#include	<ros/ros.h>

#include	<geometry_msgs/TwistStamped.h>

#ifndef EPSILON
	#define EPSILON 1.0e-5
#endif

#ifndef _ZEABUS_LIBRARY_SUBSCRIBER_SUBTWISTSTAMPED_H__
#define _ZEABUS_LIBRARY_SUBSCRIBER_SUBTWISTSTAMPED_H__

namespace zeabus_library{

namespace subscriber{

	class SubTwistStamped{

		public:
			SubTwistStamped( geometry_msgs::TwistStamped* data);

			void register_data( geometry_msgs::TwistStamped* data);
	
			void callback( const geometry_msgs::TwistStamped& message );

			void callback_ttl( const geometry_msgs::TwistStamped& message );	

			void register_ttl( int* ttl , int number = 20 );

			void set_ttl( int number );

		private:
			int constant_ttl;

			int* ttl;
			geometry_msgs::TwistStamped* data;	

	};

}

}


#endif
