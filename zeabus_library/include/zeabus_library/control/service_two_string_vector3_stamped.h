/*
	File name			:	service_two_string_vector3_stamped.h		
	Author				:	Supasan Komonlit
	Date created		:	2019 , FEB 10
	Date last modified	:	2019 , ??? ??
	Purpose				:	
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Production

	Namespace			:	zeabus_library/control
*/
//===============>

#include	<iostream>

#include	<stdio.h>

#include	<math.h>

#include	<nav_msgs/Odometry.h>

#include	<geometry_msgs/Vector3.h>

#include	<zeabus_library/tf_handle/tf_quaternion.h>

#include	<zeabus_library/TwoStringVector3Stamped.h>

#ifndef _ZEBAUS_LIBRARY_CONTROL_SERVICE_TWO_STRING_VECTOR3_STAMPED_H__
#define _ZEBAUS_LIBRARY_CONTROL_SERVICE_TWO_STRING_VECTOR3_STAMPED_H__

namespace zeabus_library{

namespace control{

	class ServiceTwoStringVector3Stamped{

		public:
			ServiceTwoStringVector3Stamped();

			void register_current( nav_msgs::Odometry* current );

			void register_target( nav_msgs::Odometry* target );

			void register_received( int* data );

			void register_velocity( bool* data_fix , double* data_velocity );

			void register_all_quaternion( zeabus_library::tf_handle::TFQuaternion* data_current
					, zeabus_library::tf_handle::TFQuaternion* data_target 
					, zeabus_library::tf_handle::TFQuaternion* data_diff );

			void callback_reset_target( 
					zeabus_library::TwoStringVector3Stamped::Request& request
					, zeabus_library::TwoStringVector3Stamped::Response& response );

			void callback_check_position(
					zeabus_library::TwoStringVector3Stamped::Request& request
					, zeabus_library::TwoStringVector3Stamped::Response& response );

			void callback_reset_velocity(
					zeabus_library::TwoStringVector3Stamped::Request& request
					, zeabus_library::TwoStringVector3Stamped::Response&  response );

		private:
			int* received_reset;

			geometry_msgs::Vector3 temp_vector3;				

			nav_msgs::Odometry* current_state;
			nav_msgs::Odometry* target_state;

			bool* fix_velocity;
			double* value_velocity;

			double temp_double[3];

			zeabus_library::tf_handle::TFQuaternion* current_quaternion;
			zeabus_library::tf_handle::TFQuaternion* target_quaternion;
			zeabus_library::tf_handle::TFQuaternion* diff_quaternion;

	};

}

}

#endif
