/*
	File name			:	SubBool.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , FEB 18
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

#include	<std_msgs/Bool.h>

#ifndef _ZEABUS_LIBRARY_SUBSCRIBER_BOOL_H__
#define _ZEABUS_LIBRARY_SUBSCRIBER_BOOL_H__

namespace zeabus_library{

namespace subscriber{

	class SubBool{

		public:
			SubBool( std_msgs::Bool* data = 0 );

			void register_data( std_msgs::Bool* data );

			void callback( const std_msgs::Bool& message );
			
			void callback_ttl( const std_msgs::Bool& message );
	
			void register_ttl( int* ttl , int number = 20 );

			void set_ttl( int number );

		private:	
			int constant_ttl;
			
			int* ttl; // ttl = time to live 
			std_msgs::Bool* data;

	};	

}

}

#endif
