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
			ListenIMUQuaternion( zeabus_library::rotation::Quaternion& quaternion 
								, boost::numeric::ublas::matrix< double >& angular_velocity
								, zeabus_library::Point3& linear_acceleration );

			void register_quaternion( zeabus_library::rotation::Quaternion& variable );
			void register_acceleration( boost::numeric::ublas::matrix< double >& variable );
			void register_gyrometer( zeabus_library::Point3& variable)

			void callback( const zeabus_library::IMUQuaternion& message );

		private:
			zeabus_library::rotation::Quaternion quaternion;
			zeabus_library::Point3 angular_velocity;
			boost::numeric::ublas::matrix< double > linear_acceleration;
	}

}

}
