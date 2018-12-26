/*
	File name			:	listen_IMUQuaternion.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 24
	Date last modified	:	2018 , DEC 27
	Purpose				:	This is header of file for listen zeabus_library/IMUQuaternion.h

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Using & Maintain
							
	Namespace			:	zeabus_library/sensor
*/

#include	<ros/ros.h>

#include	<stdio.h>

#include	<zeabus_library/rotation/rotation_handle.h>

#include	<zeabus_library/convert/IMUQuaternion.h> // include this = Point3 and Point4

#include	<zeabus_library/capture_time.h>

#include	<zeabus_library/vector.h> // include zeabus_library_matrix

#ifndef _ZEABUS_LIBRARY_SENSOR_LISTEN_IMUQUATERNION_H__
#define _ZEABUS_LIBRARY_SENSOR_LISTEN_IMUQUATERNION_H__

namespace zeabus_library{

namespace sensor{

	class ListenIMUQuaternion{

		public:
			ListenIMUQuaternion( double roll = 0 , double pitch = 0, double yaw = 0
							, double gravity = 0 );

			void callback( const zeabus_library::IMUQuaternion& message );
			void get_result( zeabus_library::IMUQuaternion& result );

			void set_orientation( double roll , double pitch , double yaw );
			void set_gravity( double gravity );

		private:
			boost::numeric::ublas::matrix< double > offset_gravity;
			boost::numeric::ublas::matrix< double > receive_acceleration;

			zeabus_library::rotation::RotationHandle RH;

			zeabus_library::IMUQuaternion receive_msg;

			zeabus_library::CaptureTime timer;

			void copy_value(  zeabus_library::IMUQuaternion target );

			boost::numeric::ublas::matrix< double > result_quaternion;
			boost::numeric::ublas::matrix< double > result_gyro;
			boost::numeric::ublas::matrix< double > result_acceleration;
	};
}
}

#endif
