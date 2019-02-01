/*
	File name			:	tf_port_dvl.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2019 , FEB 01
	Date last modified	:	2019 , FEB 01
	Purpose				:	This is file to use read connect ros and DVL

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Maintainance

	Namespace			:	None
*/

#include	<ros/ros.h>

#include	<stdio.h>

#include	<iostream>

#include	<zeabus_library/error_code.h>

#include	<zeabus_library/file.h>

#include	<zeabus_library/Point3.h>

#include	<zeabus_library/sensor/string_port.h>

#include	<tf/transform_broadcaster.h>

#include	<geometry_msgs/TwistWithCovarianceStamped.h>

#include	<std_msgs/String.h>

#define _COLLECTING_DATA_
#define _DEBUG_CODE_

#define _PUBLISH_DATA_
#define _PUBLISH_STRING_

#ifndef PI
	#define PI 3.14159265
#endif

int main( int argc , char ** argv ){

	ros::init( argc , argv , "port_dvl" );

	ros::NodeHandle nh(""); // Handle for manage about this file in ros system
	ros::NodeHandle ph("~"); // Handle for managae param from launch 

///////////////////////////////////-- PARAMETER PART --/////////////////////////////////////////

	std::string port_name;
	std::string topic_velocity; 
	std::string topic_string;
	std::string frame_id_velocity;
	double euler_ZYX[3] = { 0 , 0 , 0 };

	ph.param< std::string >("frame_id_velocity" , frame_id_velocity , "dvl_velocity");

	ph.param< std::string >("topic_velocity" , topic_velocity , "/sensor/dvl/velocity" );
	ph.param< std::string >("topic_string" , topic_string , "/sensor/dvl/string");

	ph.param< std::string >("name_port_dvl" , port_name , "/dev/usb2serial/ftdi_FT2VR5PM_02");

	ph.param< double >("offset_roll" , euler_ZYX[0] , PI );
	ph.param< double >("offset_pitch" , euler_ZYX[1] , 0.0 );
	ph.param< double >("offset_yaw" , euler_ZYX[2] , 0.0 );

//////////////////////////////////////-- TF PART --/////////////////////////////////////////////
	
	static tf::TransformBroadcaster broadcast_velocity; // linear_velocity

	tf::Quaternion tf_quaternion;
	tf_quaternion.setEulerZYX( euler_ZYX[2] , euler_ZYX[1] , euler_ZYX[0] );

	tf::Transform transform_velocity;
	transform_velocity.setRotation( tf_quaternion );

////////////////////////////////////-- ROS SYSTEM --////////////////////////////////////////////
	ros::Time time;

	geometry_msgs::TwistWithCovarianceStamped data_ros;
	std_msgs::String data_port;

	data_ros.header.frame_id = frame_id_velocity;

	ros::Publisher pub_velocity = nh.advertise< geometry_msgs::TwistWithCovarianceStamped >( 
										topic_velocity , 1 );

	ros::Publisher pub_string = nh.advertise< std_msgs::String >( topic_string , 1);

//////////////////////////////////////-- DVL PART --////////////////////////////////////////////

	#ifdef _COLLECTING_DATA_
		zeabus_library::File log_file( "zeabus_log" , "sensor/dvl" , "dvl_port" );
		size_t result_file;
		result_file = log_file.open();
		if( !( result_file == zeabus_library::NO_ERROR ) ) return zeabus_library::ERROR_ACTION;
	#endif

	zeabus_library::sensor::StringPort serial_port( port_name ) ;

	bool result;
	std::string message;
	int count = 0;

	#ifdef _DEBUG_CODE_
		printf("Try to open port\n");
	#endif

	serial_port.open_port( result );
	if( !(result) ){
		printf("Failure to open port %s\n" , port_name.c_str() );
		return -1 ;
	}

	serial_port.set_option_port( boost::asio::serial_port_base::baud_rate( 115200 ) );

	serial_port.write_data("==="); // stop device for write device
	count = 0;
	do{
		serial_port.read_data( message );
		count++;
	}while( message != "Explorer DVL" && count < 50 );
	serial_port.read_data( message );
	serial_port.read_data( message );
	serial_port.read_data( message );

	serial_port.write_data("CR1\n"); // 1 is factory and 0 is user load default value
	serial_port.read_data( message );
	serial_port.read_data( message );

	serial_port.write_data("BP001\n"); // single-ping bottom tracking 1 = enable , 0 = unable

	serial_port.write_data("BX00060\n"); // set maximum tracking depth in unit decimeter

	// set heading alignment look at sign three to heading by clock wise
	serial_port.write_data("EA09000\n"); // unit 18000 is 180.00 degree
	
	serial_port.write_data("ES35\n"); // set salinity of water ( salt is component )

	serial_port.write_data("TE00:00:00.00\n"); // set time between ensemble

	serial_port.write_data("TP00:00.00\n"); // set time between ping

	// set type of data stream 
	serial_port.write_data("PD6\n");
	
	serial_port.write_data("CK\n");
	serial_port.read_data( message );
	serial_port.read_data( message );

	serial_port.write_data("CS\n"); // continue stream

////////////////////////////////////////////////////////////////////////////////////////////////

	std::string data;
	char status;
	int temp_x , temp_y , temp_z;	

	while( nh.ok() ){
		serial_port.read_data( data );
		#ifdef _COLLECTING_DATA_
			log_file.writeline( data );
		#endif
		#ifdef _PUBLISH_STRING_
			data_port.data = data;
			pub_string.publish( data_port);
		#endif
		time = ros::Time::now();
		if( data.find(":BS") != std::string::npos ){
			sscanf( data.c_str() , ":BS,%d,%d,%d,%c" , &( temp_x ) , &( temp_y )
													, &( temp_z) , &status );
			if( status == 'A' ){
				printf( "<-------- DVL GOOD DATA ---------->\n\n");
				data_ros.twist.twist.linear.x = temp_x;// * 0.001;
				data_ros.twist.twist.linear.y = temp_y;// * 0.001;
				data_ros.twist.twist.linear.z = temp_z;// * 0.001;
				#ifdef _PUBLISH_DATA_
					data_ros.header.stamp = time;
					pub_velocity.publish( data_ros );
				#endif
				transform_velocity.setOrigin( tf::Vector3( data_ros.twist.twist.linear.x 
														, data_ros.twist.twist.linear.y
														, data_ros.twist.twist.linear.z) );
				broadcast_velocity.sendTransform( tf::StampedTransform( transform_velocity
					, time , "robot" , frame_id_velocity ) );
			}
			else{
				printf( "<-------- DVL BAD DATA ----------->\n\n");
				data_ros.twist.twist.linear.x = 0;
				data_ros.twist.twist.linear.y = 0;
				data_ros.twist.twist.linear.z = 0;
				#ifdef _PUBLISH_DATA_
					data_ros.header.stamp = time;
					pub_velocity.publish( data_ros );
				#endif
			}
		}	
		
	}

	#ifdef _COLLECTING_DATA_
		log_file.close();
	#endif

	serial_port.close_port( result );
	printf("End Action on DVL port file\n");
	return 0;

}
