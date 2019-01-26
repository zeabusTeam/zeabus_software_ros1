/*
	File name			:	listen_odometry.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , JAN 20
	Date last modified	:	2018 , ??? ??
	Purpose				:	For receive message from localize in control system
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Product

	Namespace			:	zeabus_library/control
*/

#include	<stdio.h>

#include	<iostream>

#include	<zeabus_library/Odometry.h>

#ifndef _ZEABUS_LIBRARY_CONTROL_LISTEN_ODOMETRY_H__
#define _ZEABUS_LIBRARY_CONTROL_LISTEN_ODOMETRY_H__

namespace zeabus_library{

namespace control{

	class ListenOdometry{

		public:

			ListenOdometry();

			void register_linear_position( zeabus_library::Point3* variable );
			void register_quaternion( zeabus_library::Point4* variable );
			void register_linear_velocity( zeabus_library::Point3* variable );
			void register_gyroscope( zeabus_library::Point3* variable );

			void callback( const zeabus_library::Odometry& message );

			void callback_twist( const zeabus_library::Odometry& message );
	
			void callback_state( const zeabus_library::Odometry& message );		

		protected:
			zeabus_library::Point3* linear_position;
			zeabus_library::Point4* quaternion;
			zeabus_library::Point3* linear_velocity;
			zeabus_library::Point3* gyroscope;

	};

}

}

#endif
