/*
	File name			:	robot.cpp		
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

#include	<ros/ros.h>

#include	<iostream>

#include	<tf/transform_broadcaster.h>

#include	<zeabus_library/tf_handle/tf_quaternion.h>

#include	<zeabus_library/subscriber/SubOdometry.h>

int main( int argv , char** argc ){

	ros::init( argv , argc , "robot");

	ros::NodeHandle ph("~");
	ros::NodeHandle nh("");

//====================> PARAMETER PART
	std::string frame_id;
	std::string parent_id;
	std::string subscribe_topic;
	double offset_rotation[3];
	double offset_translation[3];
	int frequency;

	ph.param< std::string >( "frame_id" , frame_id , "robot");
	ph.param< std::string >( "parent_id" , parent_id , "world");
	ph.param< std::string >( "subscribe_topic" , subscribe_topic , "/sensor/robot");

	ph.param< double >( "rotation_x" , offset_rotation[0] , 0.0 ); // roll
	ph.param< double >( "rotation_y" , offset_rotation[1] , 0.0 ); // pitch
	ph.param< double >( "rotation_z" , offset_rotation[2] , 0.0 ); // yaw
	ph.param< double >( "translation_x" , offset_translation[0] , 0.0 );
	ph.param< double >( "translation_y" , offset_translation[1] , 0.0 );
	ph.param< double >( "translation_z" , offset_translation[2] , 0.0 );

	ph.param< int >("frequency" , frequency , 100 );

//====================> TRANSFORM PART
	static tf::TransformBroadcaster broadcaster;
	
	zeabus_library::tf_handle::TFQuaternion tf_quaternion;
	tf_quaternion.setEulerZYX( offset_rotation[2] , offset_rotation[1] , offset_rotation[0] );
	tf_quaternion.normalize();

	tf::Transform transform;
	transform.setRotation( tf_quaternion );

//====================> SET UP ROS SYSTEM
	ros::Time time;
	ros::Rate rate( frequency );
	
	nav_msgs::Odometry data; // collect current state this help in rviz code

	zeabus_library::subscriber::SubOdometry listener( &data );
	ros::Subscriber sub_data = nh.subscribe( subscribe_topic , 1 
			, &zeabus_library::subscriber::SubOdometry::callback
			, &listener );
//====================> LOOP ROS SYSTEM
	while( nh.ok() ){
		rate.sleep();
		ros::spinOnce();
		time = ros::Time::now();
		transform.setOrigin(
				tf::Vector3( data.pose.pose.position.x , data.pose.pose.position.y
						, data.pose.pose.position.z ) );
		broadcaster.sendTransform( tf::StampedTransform( transform, time, parent_id, frame_id ));
	}
//====================>
//====================>



}
