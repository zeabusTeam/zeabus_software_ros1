/*
	File name			:	listen_pressure_nav.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , JAN 10
	Date last modified	:	2018 , ??? ??
	Purpose				:	For receive message from pressure
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Product

	Namespace			:	zeabus_library/localize
*/

#include	<stdio.h>

#include	<nav_msgs/Odometry.h>

#ifdef _ZEABUS_LIRABRY_LOCALIZE_LISTEN_PRESSURE_NAV_H__
#define _ZEABUS_LIRABRY_LOCALIZE_LISTEN_PRESSURE_NAV_H__

namespace zeabus_library{

namespace localize{

	class ListenPressureNav{

		public:
			ListenPressureNav( double* variable );
			void register_depth( double* variable );
			
			void callback( const nav_msgs::Odometry& message );

		private:
			double* depth;	

	};

}

}

#endif
