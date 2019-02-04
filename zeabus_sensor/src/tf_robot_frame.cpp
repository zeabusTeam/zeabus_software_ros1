/*
	File name			:	tf_robot_frame.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2018 , FEB 01
	Date last modified	:	2018 , FEB 02
	Purpose				:	This is to set connection of robot frame with world frame

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Maintainance

	Namespace			:	None
*/
//====================>

#include	<ros/ros.h>

#include	<stdio.h>

#include	<iostream>

#include	<tf/transform_broadcaster.h>

#include	<geometry_msgs/QuaternionStamped.h>

#define _DEBUG_ORDER_

class ListenQuaternionStamped{

	public: 
		ListenQuaternionStamped( geometry_msgs::QuaternionStamped* data , bool* received ){
			this->received = received;
			this->data = data;
			*(this->received) = false;
		}

		void callback( const geometry_msgs::QuaternionStamped& message ){
			*(this->received) = true;
			*(this->data) = message;
			#ifdef _DEBUG_ORDER_
				printf("Callback listen\n");
			#endif
		}

		bool* received;
		
	protected:
		geometry_msgs::QuaternionStamped* data;

};


int main( int argc , char** argv ){

	static const double zero = 0;
	static const double one = 0;

	ros::init( argc , argv , "robot_node");

	ros::NodeHandle nh("");
	ros::NodeHandle ph("~");

//====================> PARAMETER PART

	std::string topic_quaternion_01;
	

	int frequency;

	ph.param< std::string >("topic_quaternion" , topic_quaternion_01 , "/sensor/imu/quaternion");
	double offset_euler[3] = { 0 , 0 , 0 };

	ph.param< int >("frequency" , frequency , 100 );
	ph.param< double >("offset_quaternion_01_Z" , offset_euler[2] , 1.57 );
	ph.param< double >("offset_quaternion_01_Y" , offset_euler[1] , 0.0 );
	ph.param< double >("offset_quaternion_01_X" , offset_euler[0] , 0.0 );

//====================> ROS SYSTEM

	ros::Time time;
	ros::Rate rate( frequency );

	geometry_msgs::QuaternionStamped QS_01;
	bool received_01;
	ListenQuaternionStamped listener_quaternion( &QS_01 , &received_01 );
	
	ros::Subscriber sub_quaternion = nh.subscribe( topic_quaternion_01 , 1 
								, &ListenQuaternionStamped::callback , &listener_quaternion );

//====================> TRANSFORM PART 
	
	static tf::TransformBroadcaster broadcast_rotation;

	tf::Quaternion tf_quaternion;
	tf_quaternion.setValue( zero , zero , zero , one );
	tf::Quaternion temp_quaternion;

	tf::Quaternion tf_rotation;
	tf_rotation.setEulerZYX( offset_euler[2] , offset_euler[1] , offset_euler[0] );

	temp_quaternion.setEulerZYX( 0 , 0 , 0 ); 
	
	tf::Transform transform_rotation;
	transform_rotation.setOrigin( tf::Vector3(  zero , zero , zero ) );
		
//====================> LOOP ROS

	while( nh.ok() ){
		rate.sleep();
		ros::spinOnce();
		if( received_01 ){
			tf_quaternion.setValue( QS_01.quaternion.x , QS_01.quaternion.y
											,	QS_01.quaternion.z , QS_01.quaternion.w );
			
			//tf_rotation*=temp_quaternion;
			temp_quaternion*=tf_rotation;

			transform_rotation.setRotation( temp_quaternion );

			broadcast_rotation.sendTransform( tf::StampedTransform( transform_rotation 
				, QS_01.header.stamp , "world" , "robot" ) );

			received_01 = false;

			#ifdef _DEBUG_ORDER_
				printf("CHANGE ROTATION TF\n\n");
			#endif
		}
	}

}
