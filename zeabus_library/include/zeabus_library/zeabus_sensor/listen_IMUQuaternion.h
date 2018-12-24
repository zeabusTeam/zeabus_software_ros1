/*
	File name			:	listen_IMUQuaternion.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 24
	Date last modified	:	2018 , DEC ??
	Purpose				:	This is header of file for listen zeabus_library/IMUQuaternion.h

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Product
							
	Namespace			:	zeabus_sensor
*/

#include	<ros/ros.h>

#include	<stdio.h>

#include	<zeabus_library/matrix.h>

#include	<zeabus_library/zeabus_rotation/convert_rotation.h>
#include	<zeabus_library/convert_IMUData.h>
#include	<zeabus_library/capture_time.h>

#ifndef _ZEABUS_LIBRARY_ZEABUS_SENSOR_LISTEN_IMUDATA_H__
#define _ZEABUS_LIBRARY_ZEABUS_SENSOR_LISTEN_IMUDATA_H__

namespace zeabus_sensor{

	class ListenIMUData{

		public:
			ListenIMUData( double roll = 0 , double pitch = 0, double yaw = 0
							, double gravity = 0 );

			void callback( const zeabus_library::IMUQuaternion& message );
			void get_result( zeabus_library::IMUQuaternion& result );

			void set_orientation( double roll , double pitch , double yaw );
			void set_gravity( double gravity );

		private:
			zeabus_library::zeabus_rotation::QuaternionHandle QH;

			zeabus_library::CaptureTime timer;
	};

}

#endif
