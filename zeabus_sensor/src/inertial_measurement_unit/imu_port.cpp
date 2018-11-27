/*
	File name			:	imu_port.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2018 , NOV 27
	Date last modified	:	2018 , ??? ??
	Purpose				:	This is file to use read connect ros and IMU

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.0.1
	status				:	Production

	Namespace			:	None
*/

#include	<ros/ros.h>

#include	<zeabus_library/zeabus_sensor/imu_command.h>

int main( int argv , char** argc ){
	
	ros::init( argv , argc , "main_control");

	ros::NodeHandle ph(""); // Handle for manage param from launch

	std::string port_name = "/dev/microstrain/3dm_gx5_45_0000__6251.65901";

	zeabus_sensor::IMUPort imu( port_name );

	bool result ;

	imu.open_port( result );

}
