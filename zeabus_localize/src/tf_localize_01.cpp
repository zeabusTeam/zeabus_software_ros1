/*
	File name			:	tf_localize_01.cpp
	Author				:	Supasan Komonlit	
	Date created		:	2019 , FEB 06
	Date last modified	:	2019 , ??? ??
	Purpose				:	For localize by DVL , IMU 1 , Pressure 

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Using

	Namespace			:	None
*/
//====================>

#include	<stdio.h>

#include	<ros/ros.h>

#include	<tf/transform_listener.h>

#include	<zeabus_library/tf_handle/tf_quaternion.h>

#include	<zeabus_library/text_color.h>

#include	<zeabus_library/subscriber/SubImu.h>
#include	<zeabus_library/subscriber/SubOdometry.h>
#include	<zeabus_library/subscriber/SubTwistWithCovarianceStamped.h>

int main( int argv , char** argc ){

	ros::init( argv , argc , "localize" );

	ros::NodeHandle ph("~");
	ros::NodeHandle nh("");

//====================> PARAMETER PART
	std::string publish_topic, frame_id;
	std::string topic_imu , id_imu;
	std::string topic_dvl , id_dvl;
	std::string topic_pressure;
	int frequency;

	ph.param< std::string >("publich_topic" , publish_topic , "/localize/state");
	ph.param< std::string >("topic_imu" , topic_imu , "/sensor/imu");
	ph.param< std::string >("topic_dvl" , topic_dvl , "/sensor/dvl");
	ph.param< std::string >("topic_pressure" , topic_pressure , "/sensor/pressure/node");
	ph.param< std::string >("id_imu" , id_imu , "imu");
	ph.param< std::string >("id_dvl" , id_dvl , "dvl");
	ph.param< std::string >("frame_id" , frame_id , "world");
	ph.param< int >("frequency" , frequency , 50 );
		
//====================> TRANSFORMATION PART
	bool success = false;

	zeabus_library::tf_handle::TFQuaternion imu_quaternion;
	zeabus_library::tf_handle::TFQuaternion dvl_quaternion;
	
	tf::TransformListener listener; // variable for listen about transform
	tf::StampedTransform transform; // for receive data form listener

	printf( "Waiting receive rotation imu -> world ====> ");
	while( nh.ok() ){
		try{
			listener.lookupTransform( frame_id , id_imu , ros::Time(0) , transform );
			zeabus_library::normal_green( " SUCCESS\n");
			success = true;
			break;
		}
		catch( tf::TransformException &ex) {
			zeabus_library::bold_red( ex.what() );
			printf("\n");
			printf("\tWaiting --");
			ros::Duration(1.0).sleep();
			printf(" -- try again ====>");
			continue;
		}
	}

	if( ! success ) return -1;	


}
