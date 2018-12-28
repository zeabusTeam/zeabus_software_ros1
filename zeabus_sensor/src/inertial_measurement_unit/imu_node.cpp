/*
	File name			:	imu_node.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 05
	Date last modified	:	2018 , DEC 27
	Purpose				:	This is file to filter convert value before send to next node

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.6.0
	status				:	Product

	Namespace			:	None
*/

#include	<ros/ros.h>

#include	<zeabus_library/IMUData.h>

#include	<zeabus_library/euler.h>

#include	<zeabus_library/sensor/listen_IMUQuaternion.h>

int main( int argv , char** argc ){

	ros::init( argv , argc , "node_imu");

	ros::NodeHandle nh(""); // Handle for manage about this file in ros system
	ros::NodeHandle ph("~"); // Handle for get param from launch file

//====================================== PARAMTER PART ========================================//
	std::string topic_input;
	std::string topic_output;
	int frequency;

	ph.param< std::string >( "topic_input_node_imu" , topic_input , "sensor/imu/port/zeabus" );
	ph.param< std::string >( "topic_output_node_imu" , topic_output , "sensor/imu/node" );
	ph.param< int >("frequency_imu" , frequency , 100 );

	printf("Bofore Init variable listener\n");
	zeabus_library::sensor::ListenIMUQuaternion listener( 
												zeabus_library::euler::degree_domain( 0 ) 
												, zeabus_library::euler::degree_domain( 0 ) 
												, zeabus_library::euler::degree_domain( -90 ) 
												, 1 );	
	printf("After Init variable listener\n");

	ros::Subscriber sub_IMUData = nh.subscribe( topic_input , 1 
									, &zeabus_library::sensor::ListenIMUQuaternion::callback
									, &listener );
	printf("Now already listen\n");
	ros::spin();
}
