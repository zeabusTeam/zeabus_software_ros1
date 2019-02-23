/*
	File name			:	ServiceThreeOdometry.h		
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

#include	<zeabus_library/ThreeOdometry.h>

#ifndef _ZEABUS_LIBRARY_CONTROL_SERVICE_THREE_ODOMETRY_H__
#define _ZEABUS_LIBRARY_CONTROL_SERVICE_THREE_ODOMETRY_H__

namespace zeabus_library{

namespace control{

	class ServiceThreeOdometry{

		public:

			ServiceThreeOdometry();
			
			void register_all_state( nav_msgs::Odometry* data_current 
					, nav_msgs::Odometry* data_target );

			void register_current( nav_msgs::Odometry* data);

			void register_target( nav_msgs::Odometry* data);

			bool callback_get_state( zeabus_library::ThreeOdometry::Request& request
					, zeabus_library::ThreeOdometry::Response& response );

		private:

			nav_msgs::Odometry* current_state;
			nav_msgs::Odometry* target_state;

	};

}

}

#endif
