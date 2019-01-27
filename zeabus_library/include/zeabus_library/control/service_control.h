/*
	File name			:	service_control.h		
	Author				:	Supasan Komonlit
	Date created		:	2019 , JAN 29
	Date last modified	:	2019 , JAN 29
	Purpose				:	This for manage about all service to connect with control
							This file will connect many variable of control please enure to all
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Production

	Namespace			:	zeabus_library/control
*/

#include	<ros/ros.h>

#include	<stdio.h>

#include	<iostream>

#include	<zeabus_library/Point3.h>

#include	<zeabus_library/Point4.h>

#ifndef _ZEABUS_LIBRARY_CONTROL_SERVICE_CONTROL_H__
#define _ZEABUS_LIBRARY_CONTROL_SERVICE_CONTROL_H__

namespace zeabus_library{

namespace control{

	class ServiceControl{

		public:
			
			ServiceControl( int* mode );	// single interger not array 

			register_position( zeabus_library::Point3* current , zeabus_library::Point3* target);
			register_euler( double* current , double* target );
			register_quaternion( zeabus_library::Point4* current 
								, zeabus_library::Point4* target);
			register_mode( int* mode );

			


		protected:
			
			int* mode;

			zeabus_lirary::Point3* current_position;
			zeabus_lirary::Point3* target_position;

			zeabus_lirary::Point4* current_position;
			zeabus_lirary::Point4* target_position;

			double* current_euler;
			double* target_euler;

	};

}

}

#endif
