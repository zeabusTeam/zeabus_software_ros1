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
#include	<tf/transform_listener.h>

#include	<zeabus_library/tf_handle/tf_quaternion.h>

#include	<zeabus_library/text_color.h>

#include	<zeabus_library/subscriber/SubOdometry.h>
#include	<zeabus_library/subscriber/SubImu.h>

int main( int argv , char** argc ){

	double zero = 0;
	double one = 1;

	ros::init( argv , argc , "robot");

	ros::NodeHandle ph("~");
	ros::NodeHandle nh("");

//====================> PARAMETER PART
	std::string frame_id;
	std::string parent_id;
	std::string imu_id;
	std::string topic_state;
	std::string topic_imu;
	int frequency;

	ph.param< std::string >( "frame_id" , frame_id , "robot");
	ph.param< std::string >( "parent_id" , parent_id , "world");
	ph.param< std::string >( "imu_id" , imu_id , "imu");
	ph.param< std::string >( "subscribe_topic" , topic_state , "/localize/state");
	ph.param< std::string >( "subscribe_imu" , topic_imu , "/sensor/imu");

	ph.param< int >("frequency" , frequency , 100 );

//====================> TRANSFORM PART
	static tf::TransformBroadcaster broadcaster;
	
	bool success = false;
	zeabus_library::tf_handle::TFQuaternion rotation_imu( zero , zero , zero , one );
	tf::Transform transform;
	tf::TransformListener tf_listener;
	tf::StampedTransform stamped_transform;
	transform.setRotation( rotation_imu ); // this is temp set can run out of problem

	printf( "Waiting receive rotation imu -> robot ===> ");
	while( nh.ok() && (!success) ){
		try{
			tf_listener.lookupTransform( frame_id, imu_id, ros::Time(0), stamped_transform );
			zeabus_library::normal_green( "SUCCESS\n");
			success = true;
			rotation_imu = stamped_transform.getRotation();
			break;
		}
		catch( tf::TransformException &ex ){
			zeabus_library::bold_red( ex.what() );
			printf("\n\tWaiting --");
			ros::Duration(1.0).sleep();
			printf(" -- try again ====> ");
			continue;
		}
	}

	if( ! success ) return -1;
	
//====================> SET UP ROS SYSTEM
	ros::Time time;
	ros::Rate rate( frequency );
	
	nav_msgs::Odometry current_state; // collect current state this help in rviz code
	sensor_msgs::Imu data_imu; 
	int received_imu = 0;

	zeabus_library::subscriber::SubOdometry listener_state( &current_state );
	ros::Subscriber sub_data = nh.subscribe( topic_state , 1 
			, &zeabus_library::subscriber::SubOdometry::callback , &listener_state );

	zeabus_library::subscriber::SubImu listener_imu( &data_imu );
	listener_imu.register_ttl( &received_imu , 1 );
	ros::Subscriber sub_imu = nh.subscribe( topic_imu , 1 
			, &zeabus_library::subscriber::SubImu::callback_ttl , &listener_imu );
//====================> LOOP ROS SYSTEM
	while( nh.ok() ){
		rate.sleep();
		ros::spinOnce();
		time = ros::Time::now();
		if( received_imu ){
			zeabus_library::tf_handle::TFQuaternion tf_quaternion( data_imu.orientation );
			tf_quaternion = rotation_imu*tf_quaternion;
			transform.setRotation( tf_quaternion );	
//			printf("ROBOT EULER	: "); tf_quaternion.print_radian(); printf("\n");	
			received_imu = 0;
		}
		transform.setOrigin( tf::Vector3( current_state.pose.pose.position.x 
				, current_state.pose.pose.position.y , current_state.pose.pose.position.z ) );
		broadcaster.sendTransform( tf::StampedTransform( transform, time, parent_id, frame_id ));
	}

}
