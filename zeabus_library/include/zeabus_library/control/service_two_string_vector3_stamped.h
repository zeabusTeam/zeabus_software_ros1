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

#include	<nav_msgs/Odometry.h>

#include	<zeabus_library/tf_handle/TFQuaternion.h>

#include	<zeabus_library/TwoStringVecotr3Stamped.h>

#include	<zeabus_library/tf_handle/TFQuaternion.h>

#ifndef _ZEBAUS_LIBRARY_CONTROL_SERVICE_TWO_STRING_VECTOR3_STAMPED_H__
#define _ZEBAUS_LIBRARY_CONTROL_SERVICE_TWO_STRING_VECTOR3_STAMPED_H__

namespace zeabus_library{

namespace control{

	class ServiceTwoStringVector3Stamped{

		public:
			ServiceTwoStringVecotr3Stamped():

			void register_current( nav_msgs::Odometry* current );

			void register_target( nav_msgs::Odometry* target );

			void register_received( int* data );

			void callback_reset_target( 
					zeabus_library::TwoStringVecotr3Stamped::Request& request
					, zeabus_library::TwoStringVecotr3Stamped::Response& response );

			void callback_check_position(
					zeabus_library::TwoStringVecotr3Stamped::Request& request
					, zeabus_library::TwoStringVecotr3Stamped::Response& response );

		private:
			int* received_reset;
				
			nav_msgs::Odometry* current_state;
			nav_msgs::Odometry* target_state;


	};

}

}

#endif
