/*
	File name			:	dvl.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2019 , FEB 06
	Date last modified	:	2019 , FEB 06
	Purpose				:	

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Maintainance

	Namespace			:	None
*/
//====================>

//#define _TEST_CONNECTION_ // If define this line. This code willn't connect dvl hardware

#include	<ros/ros.h>

#include	<stdio.h>

#include	<iostream>

#include	<tf/transform_broadcaster.h>

#include	<zeabus_library/error_code.h>

#include	<zeabus_library/file.h>

#include	<zeabus_library/sensor/string_port.h>

#include	<zeabus_library/tf_handle/tf_quaternion.h>

#include	<std_msgs/String.h>

#ifdef _TEST_CONNECTION_
	#include	<zeabus_library/subscriber/SubTwistWithCovarianceStamped.h>
#endif

#include	<geometry_msgs/TwistWithCovarianceStamped.h>

#ifndef _TEST_CONNECTION_
	#define _COLLECTING_DATA_
#endif

#define _DEBUG_CODE_

#ifndef _TEST_CONNECTION_
	#define _PUBLISH_RAW_DATA_
#endif

int main( int argc , char ** argv ){

	ros::init( argc , argv , "dvl" );

	ros::NodeHandle nh(""); // Handle for manage about this file in ros system
	ros::NodeHandle ph("~"); // Handle for managae param from launch 

//====================> PARAMETER PART
	std::string port_name =  "/dev/usb2serial/ftdi_FT2VR5PM_02";
	std::string publish_topic;
	std::string frame_id;
	std::string parent_id;
#ifdef _TEST_CONNECTION_
	std::string subscribe_topic;
#endif
#ifdef _PUBLISH_RAW_DATA_
	std::string raw_topic;
#endif
	double offset_rotation[3];
	double offset_translation[3];

	ph.param< std::string >("port_dvl" , port_name , "/dev/usb2serial/ftdi_FT2VR5PM_02");

	ph.param< std::string >("publish_topic" , publish_topic , "/sensor/dvl" );
#ifdef _TEST_CONNECTION_
	ph.param< std::string >("subscribe_topic", subscribe_topic , "/test/dvl");
#endif
#ifdef _PUBLISH_RAW_DATA_
	ph.param< std::string >("raw_topic", raw_topic , "/raw/dvl");
#endif
	ph.param< std::string >("frame_id" , frame_id , "dvl");
	ph.param< std::string >("parent_id" , parent_id , "robot");

	ph.param< double >( "rotation_x" , offset_rotation[0] , 0.0 ); // roll
	ph.param< double >( "rotation_y" , offset_rotation[1] , 0.0 ); // pitch
	ph.param< double >( "rotation_z" , offset_rotation[2] , 0.0 ); // yaw
	ph.param< double >( "translation_x" , offset_translation[0] , 0.0 );
	ph.param< double >( "translation_y" , offset_translation[1] , 0.0 );
	ph.param< double >( "translation_z" , offset_translation[2] , 0.0 );

//====================> TRANSFORM PART

	static tf::TransformBroadcaster broadcaster;

	zeabus_library::tf_handle::TFQuaternion tf_quaternion;
	tf_quaternion.setEulerZYX( offset_rotation[2] , offset_rotation[1] , offset_rotation[0] );
	tf_quaternion.normalize();

	tf::Transform transform;
	transform.setOrigin( 
			tf::Vector3( offset_translation[0] , offset_translation[1] , offset_rotation[2]) );
	transform.setRotation( tf_quaternion );

//====================> SETUP ROS SYSTEM

	geometry_msgs::TwistWithCovarianceStamped sensor;
	sensor.header.frame_id = frame_id;

	ros::Publisher pub_sensor = nh.advertise< geometry_msgs::TwistWithCovarianceStamped >( 
			publish_topic , 1 );

#ifdef _PUBLISH_RAW_DATA_
	std_msgs::String raw_data;
	ros::Publisher pub_raw = nh.advertise< std_msgs::String >( raw_topic , 1 );
#endif

#ifdef _TEST_CONNECTION_
	zeabus_library::subscriber::SubTwistWithCovarianceStamped listener( &sensor );
	ros::Subscriber sub_sensor = nh.subscribe( subscribe_topic , 1 
			, &zeabus_library::subscriber::SubTwistWithCovarianceStamped::callback
			, &listener );
#endif

	ros::Rate rate(100);

//====================> OTHER

	#ifdef _COLLECTING_DATA_
		zeabus_library::File log_file( "zeabus_log" , "sensor/dvl" , "dvl_port" );
		size_t result_file;
		result_file = log_file.open();
		if( !( result_file == zeabus_library::NO_ERROR ) ) return zeabus_library::ERROR_ACTION;
	#endif

//====================> CONNECTION TO DVL
#ifndef _TEST_CONNECTION_
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
#endif
//====================> END PART CONNECT HARDWARE DVL

	std::string data;
	char status;
	int temp_x , temp_y , temp_z;	
	ros::Time time;
//====================> START LOOP IN ROS SYSTEM

	while( nh.ok() ){

#ifndef _TEST_CONNECTION_
		serial_port.read_data( data );
		#ifdef _PUBLISH_RAW_DATA_
			raw_data.data = data;
			pub_raw.publish( raw_data );
		#endif
		#ifdef _COLLECTING_DATA_
			log_file.writeline( data );
		#endif
		if( data.find(":BS") != std::string::npos ){
			sscanf( data.c_str() , ":BS,%d,%d,%d,%c" , &( temp_x ) , &( temp_y )
													, &( temp_z) , &status );
			if( status == 'A' ){
				printf( "<-------- DVL GOOD DATA ---------->\n\n");
				sensor.twist.twist.linear.x = temp_x;
				sensor.twist.twist.linear.y = temp_y;
				sensor.twist.twist.linear.z = temp_z;
				time = ros::Time::now();	
				sensor.header.stamp = time;
				pub_sensor.publish( sensor );
				broadcaster.sendTransform(
						tf::StampedTransform( transform , time , parent_id , frame_id ) );
			}
			else{
				printf( "<-------- DVL BAD DATA ----------->\n\n");
			}
		}
#else
		rate.sleep();
		ros::spinOnce();
		time = ros::Time::now();
		sensor.header.stamp = time;
		sensor.header.frame_id = frame_id;
		pub_sensor.publish( sensor );
		broadcaster.sendTransform( tf::StampedTransform( transform, time, parent_id, frame_id ));

#endif	
		
	}

	#ifdef _COLLECTING_DATA_
		log_file.close();
	#endif

#ifndef _TEST_CONNECTION_
	serial_port.close_port( result );
	printf("End Action on DVL port file\n");
#endif
	return 0;

}
