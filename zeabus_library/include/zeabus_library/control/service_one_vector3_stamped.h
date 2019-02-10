/*
	File name			:	service_one_vector3_stamped.h		
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

#include	<stdio.h>

#include	<iostream>

#include	<nav_msgs/Odometry.h>

#include	<zeabus_library/linear_quaternion.h>

#include	<zeabus_library/tf_handle/TFQuaternion.h>

#include	<zeabus_library/OneVector3Stamped.h>

#ifndef _ZEABUS_LIBRARY_CONTROL_SERVICE_ONE_VECTOR3_STAMPED_H__
#define _ZEABUS_LIBRARY_CONTROL_SERVICE_ONE_VECTOR3_STAMPED_H__

namespace zeabus_library{

namespace control{

	class ServiceOneVector3Stamped{

		public:
			ServiceOneVector3Stamped();

			void register_all_state( nav_msgs::Odometry* data_current 
					, nav_msgs::Odometry* data_target , nav_msgs::Odometry* data_linear );

			void register_current( nav_msgs::Odometry* data);

			void register_target( nav_msgs::Odometry* data);

			void register_linear( nav_msgs::Odometry* data);

			void register_equation( zeabus_library::linear_quaternion* data);

			void register_velocity( bool* data_fix , double* data_velocity );

			void callback_relative_xy( zeabus_library::OneVector3Stamped::Request& request
					, zeabus_library::OneVector3Stamped::Response& response );

			void callback_relative_z( zeabus_library::OneVector3Stamped::Response& request
					, zeabus_library::OneVector3Stamped::Response& response );

			void callback_relative_yaw( zeabus_library::OneVector3Stamped::Response& request
					, zeabus_library::OneVector3Stamped::Response& response );

			void callback_fix_yaw( zeabus_library::OneVector3Stamped::Response& request
					, zeabus_library::OneVector3Stamped::Response& response );

			void callback_fix_z( zeabus_library::OneVector3Stamped::Response& request
					, zeabus_library::OneVector3Stamped::Response& response );

			void callback_velocity_xy( zeabus_library::OneVector3Stamped::Response& request
					, zeabus_library::OneVector3Stamped::Response& response );

			void callback_velocity_z( zeabus_library::OneVector3Stamped::Response& request
					, zeabus_library::OneVector3Stamped::Response& response );

			void callback_velocity_yaw( zeabus_library::OneVector3Stamped::Response& request
					, zeabus_library::OneVector3Stamped::Response& response );

		private:
			nav_msgs::Odometry* current_state;
			nav_msgs::Odometry* target_state;
			nav_msgs::Odometry* linear_state;

			zeabus_library::LinearEquation* equation;

			bool fix_velocity[6];
			double value_velocity[6];

	};

}

}

#endif
