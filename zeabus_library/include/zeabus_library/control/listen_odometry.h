/*
	File name			:	listen_odometry.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , JAN 20
	Date last modified	:	2019 , JAN 29
	Purpose				:	For receive message from localize in control system
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.1.0
	status				:	Using

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

			void register_target_position( zeabus_library::Point3* variable );
			void register_target_quaternion( zeabus_library::Point4* variable );
	
			void register_all( zeabus_library::Point3* current_position
							, zeabus_library::Point4* current_quaternion 
							, zeabus_library::Point3* current_velocity_linear 
							, zeabus_library::Point3* current_velocity_angular
							, zeabus_library::Point3* target_position
							, zeabus_library::Point4* target_quaternion );

			void callback( const zeabus_library::Odometry& message );

			void callback_twist( const zeabus_library::Odometry& message );
	
			void callback_state( const zeabus_library::Odometry& message );		

			bool first_time;


		protected:
			zeabus_library::Point3* linear_position;
			zeabus_library::Point4* quaternion;
			zeabus_library::Point3* linear_velocity;
			zeabus_library::Point3* gyroscope;
			zeabus_library::Point3* target_linear_position;
			zeabus_library::Point4* target_quaternion;


	};

}

}

#endif
