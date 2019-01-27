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

#include	<zeabus_library/GetTwoPoint.h>

#include	<zeabus_library/OneInt.h>

#include	<zeabus_library/StrPoint.h>

#include	<zeabus_library/rotation/quaternion.h>

#include	<zeabus_library/TwoPoint.h>

#include	<zeabus_library/GetTwoPoint.h>

#include	<math.h>

#include	<zeabus_library/euler.h>

#ifndef _ZEABUS_LIBRARY_CONTROL_SERVICE_CONTROL_H__
#define _ZEABUS_LIBRARY_CONTROL_SERVICE_CONTROL_H__

namespace zeabus_library{

namespace control{

	class ServiceControl{

		public:
			
			ServiceControl( int* mode );	// single interger not array 

			void register_position( zeabus_library::Point3* current 
									, zeabus_library::Point3* target
									, zeabus_library::Point3* diff);

			void register_euler( double* current , double* target , double* diff );

			void register_quaternion( zeabus_library::Point4* current 
								, zeabus_library::Point4* target);

			void register_mode( int* mode );

			void register_ok_error( double* ok_error );

			bool callback_relative_position( zeabus_library::TwoPoint::Request& request
											,zeabus_library::TwoPoint::Response& response );
	
			bool callback_mode_control( zeabus_library::OneInt::Request& request 
									,	zeabus_library::OneInt::Response& response );

			bool callback_fix_point( zeabus_library::StrPoint::Request& request 
									, zeabus_library::StrPoint::Response& response );

			bool callback_check_position( zeabus_library::StrPoint::Request& request 
									, zeabus_library::StrPoint::Response& response );

			bool callback_get_target( zeabus_library::GetTwoPoint::Request& request 
									, zeabus_library::GetTwoPoint::Response& response );

		
		protected:
			
			int* mode;

			zeabus_library::Point3* current_position;
			zeabus_library::Point3* target_position;
			zeabus_library::Point3* diff_position;

			zeabus_library::Point4* current_quaternion;
			zeabus_library::Point4* target_quaternion;

			zeabus_library::rotation::Quaternion quaternion;

			double* current_euler;
			double* target_euler;
			double* diff_euler;
			
			double* ok_error;

	};

}

}

#endif
