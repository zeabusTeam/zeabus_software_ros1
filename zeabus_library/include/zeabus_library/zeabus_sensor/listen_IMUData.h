/*
	File name			:	listen_IMUData.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 06
	Date last modified	:	2018 , ??? 06
	Purpose				:	This is header of file for listen zeabus_library/IMUData.h

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.0.1
	status				:	Product
							
	Namespace			:	zeabus_sensor
*/

#include	<ros/ros.h>

#include	<stdio.h>

#include	<zeabus_library/IMUData.h>

#include	<zeabus_library/convert_rotation.h>
#include	<zeabus_library/convert_IMUData.h>
#include	<zeabus_library/capture_time.h>
#ifndef _ZEABUS_LIBRARY_ZEABUS_SENSOR_LISTEN_IMUDATA_H__
#define _ZEABUS_LIBRARY_ZEABUS_SENSOR_LISTEN_IMUDATA_H__

namespace zeabus_sensor{

	class ListenIMUData{

		public:
			ListenIMUData( double roll = 0 , double pitch = 0, double yaw = 0
							, double gravity = 0 );

			void callback( const zeabus_library::IMUData& message );
			void get_result( zeabus_library::IMUData& result );

			void set_orientation( double roll , double pitch , double yaw );
			void set_gravity( double gravity );

		private:
			// for collecting matrix transform
			boost::numeric::ublas::matrix< double > matrix_imu_to_robot;
			boost::numeric::ublas::matrix< double > matrix_imu_to_world;
			boost::numeric::ublas::matrix< double > matrix_robot_to_imu;
			boost::numeric::ublas::matrix< double > matrix_world_to_imu;
			// for collecting specific data
			boost::numeric::ublas::matrix< double > offset_euler;
			boost::numeric::ublas::matrix< double > offset_gravity;
			// for collecting matrix receive data
			boost::numeric::ublas::matrix< double > receive_euler;
			boost::numeric::ublas::matrix< double > receive_gyro;
			boost::numeric::ublas::matrix< double > receive_acceleration;
			// for collecting matrix result for get data
			boost::numeric::ublas::matrix< double > result_euler;
			boost::numeric::ublas::matrix< double > result_gyro;
			boost::numeric::ublas::matrix< double > result_acceleration;
			// for collection temporary matrix
			boost::numeric::ublas::matrix< double > temporary_matrix;
			zeabus_library::MatrixRotation matrix_handle;

			zeabus_library::CaptureTime timer;
	};

}

#endif
