/*
	File name			:	SubTwistWithCovarianceStamped.h		
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

#include	<geometry_msgs/TwistWithCovarianceStamped.h>

#ifndef _ZEABUS_LIBRARY_SUBSCRIBER_SEBTWISTWITHCOVARIANCESTAMPED_H__
#define _ZEABUS_LIBRARY_SUBSCRIBER_SEBTWISTWITHCOVARIANCESTAMPED_H__

namespace zeabus_library{

namespace subscriber{

	class SubTwistWithCovarianceStamped{

		public:	
			SubTwistWithCovarianceStamped( geometry_msgs::TwistWithCovarianceStamped* data );

			void register_data( geometry_msgs::TwistWithCovarianceStamped* data );

			void callback( const geometry_msgs::TwistWithCovarianceStamped& message );

			void callback_ttl( const geometry_msgs::TwistWithCovarianceStamped& message );
	
			void register_ttl( int* ttl , int number = 20 );

			void set_ttl( int number );

		private:
			int constant_ttl;
			
			int* ttl; // ttl = time to live 
			geometry_msgs::TwistWithCovarianceStamped* data;


	};

}

}

#endif
