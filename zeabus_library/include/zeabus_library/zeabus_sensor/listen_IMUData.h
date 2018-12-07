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

namespace zeabus_sensor{

	class listen_IMUData{

		public:
			listen_IMUData( double roll , double pitch , double yaw );

			void callback( const zeabus_library::IMUData& result );
			void get_result( zeabus_library::IMUData& result );

		private:
			boost::numeric::ublas::matrix< double > matrix_imu_to_robot;
			boost::numeric::ublas::matrix< double > matrix_imu_to_world;
			boost::numeric::ublas::matrix< double > result_euler;
			boost::numeric::ublas::matrix< double > result_gyro;
			boost::numeric::ublas::matrix< double > result_acceleration;
	};

}
