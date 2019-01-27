/*
	File name			:	listen_twist.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , JAN 20
	Date last modified	:	2018 , ??? ??
	Purpose				:	For receive target velocity in control system
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Product

	Namespace			:	zeabus_library/control
*/

#include	<zeabus_library/Twist.h>

#include	<zeabus_library/Point3.h>

#include	<zeabus_library/general.h>

#include	<iostream>

#include	<stdio.h>

#ifndef _ZEABUS_LIBRARY_CONTROL_LISTEN_TWIST_H__
#define _ZEABUS_LIBRARY_CONTROL_LISTEN_TWIST_H__

namespace zeabus_library{

namespace control{

	class ListenTwist{

		public:
			ListenTwist( int* count );

			void set_constant( int number );			

			void register_all( zeabus_library::Point3* linear , zeabus_library::Point3* angular);

			void register_linear( zeabus_library::Point3* variable );

			void register_angular( zeabus_library::Point3* variable );

			void callback( const zeabus_library::Twist& message );

			void callback_linear( const zeabus_library::Twist& message );

			void callback_angular( const zeabus_library::Twist& message );

		protected:
			int constant;
			int* set_count;
			zeabus_library::Point3* linear;
			zeabus_library::Point3* angular;
	
			void check_zero_linear();
			void check_zero_angular();

	};

}

}

#endif
