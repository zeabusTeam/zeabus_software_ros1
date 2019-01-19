/*
	File name			:	listen_IMUQuaternion.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , JAN 29
	Date last modified	:	2018 , ??? ??
	Purpose				:	For count_time
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Product

	Namespace			:	zeabus_library/localize
*/

#include	<ros/ros.h>

#include	<stdio.h>

#include	<zeabus_library/vector.h>

#include	<zeabus_library/Point3.h>

#include	<zeabus_library/Point4.h>

#include	<zeabus_library/listen_basic.h>

#include	<zeabus_library/IMUQuaternion.h>

#include	<zeabus_library/rotation/quaternion.h>

#ifndef _ZEABUS_LIBRARY_LOCALIZE_LISTEN_IMUQUATERNION_H__
#define _ZEABUS_LIBRARY_LOCALIZE_LISTEN_IMUQUATERNION_H__

namespace zeabus_library{

namespace localize{

	class ListenIMUQuaternion : public zeabus_library::ListenBasic{
		
		public:
			ListenIMUQuaternion( zeabus_library::Point4* quaternion 
								, zeabus_library::Point3* angular_velocity
								, zeabus_library::Point3* linear_acceleration );

			void register_quaternion( zeabus_library::Point4* variable );
			void register_gyrometer( zeabus_library::Point3* variable );
			void register_acceleration( zeabus_library::Point3* variable);

			void callback( const zeabus_library::IMUQuaternion& message );

			void callback_quaternion( const zeabus_library::IMUQuaternion& message);

		private:
			zeabus_library::Point4* quaternion;
			zeabus_library::Point3* linear_acceleration;
			zeabus_library::Point3* angular_velocity;
	};

}

}

#endif
