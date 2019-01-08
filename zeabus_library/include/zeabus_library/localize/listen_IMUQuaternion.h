/*
	File name			:	listen_IMUQuaternion.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 29
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

#include	<zeabus_library/IMUQuaternion.h>

#include	<zeabus_library/rotation/quaternion.h>

namespace zeabus_library{

namespace localize{

	class ListenIMUQuaternion{
		
		public:
			ListenIMUQuaternion( zeabus_library::rotation::Quaternion& variable );

			void register_result( zeabus_library::rotation::Quaternion& variable );

			void callback( const zeabus_library::IMUQuaternion& message );


	}

}

}
