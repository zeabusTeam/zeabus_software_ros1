/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File	: filter_imu_node.cpp
//
//	Create	: Oct 12 , 2018
//	Author	: Supasan Komonlit
//
//	Main Purpose : filter and choose node of data form LORD MICROSTRIN SENSOR 3dm_gx5_45
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include	<iostream>
#include	<geometry_msgs/TwistStamped.h>
#include	<geometry_msgs/Vector3Stamped.h>
#include	<ros/ros.h>
#include	<vector>
#include	<stdio.h>
#include	<stdlib.h>
#include	<math.h>
#define TEST_ROS_SYSTEM
#define PI  3.141592653589793;

double temporary;
double adding_yaw;
double adding_roll;
double adding_pitch;
double error_velocity;
int max_data_of_velocity;
int max_data_of_euler;
int current_data_velocity;
int current_data_euler;
geometry_msgs::TwistStamped data_velocity;
geometry_msgs::TwistStamped message_velocity;
geometry_msgs::Vector3Stamped message_euler;

void listen_velocity( const geometry_msgs::TwistStamped message){
	if( current_data_velocity == 0){
		data_velocity.twist.linear.x = message.twist.linear.x;
		data_velocity.twist.linear.y = message.twist.linear.y;
		data_velocity.twist.linear.z = message.twist.linear.z;
		data_velocity.twist.angular.x = message.twist.angular.x;
		data_velocity.twist.angular.y = message.twist.angular.y;
		data_velocity.twist.angular.z = -1*message.twist.angular.z;
		std::cout << "IN ZERO\n";
	}
	else{
		data_velocity.twist.linear.x += message.twist.angular.x;
		data_velocity.twist.linear.y += message.twist.angular.y;
		data_velocity.twist.linear.z += message.twist.angular.z;
		data_velocity.twist.angular.x += message.twist.angular.x;
		data_velocity.twist.angular.y += message.twist.angular.y;
		data_velocity.twist.angular.z += -1*message.twist.angular.z;
	}
	current_data_velocity++;
}

void listen_euler( const geometry_msgs::Vector3Stamped message){
	message_euler.vector.x = message.vector.x;
	message_euler.vector.y = message.vector.y;
	message_euler.vector.z =-1*message.vector.z;
}

void set_meesage_velocity(){
	if(current_data_velocity == 0 ) return;
	data_velocity.twist.linear.x /= current_data_velocity;
	data_velocity.twist.linear.y /= current_data_velocity;
	data_velocity.twist.linear.z /= current_data_velocity;
	data_velocity.twist.angular.x /= current_data_velocity;
	data_velocity.twist.angular.y /= current_data_velocity;
	data_velocity.twist.angular.z /= current_data_velocity;
	std::cout << "linear x : " << data_velocity.twist.linear.x << "\n";
	std::cout << "linear y : " << data_velocity.twist.linear.y << "\n";
	temporary = 1 *( data_velocity.twist.linear.x * cos(adding_yaw)
										+ data_velocity.twist.linear.y * sin(adding_yaw) 
									);
	std::cout << "temporary of x is " << std::dec << temporary << "\n";
	if( fabs(temporary) < 0.5 && fabs(temporary) >= 0.01){ 
		std::cout << "Add value to x\n";
		message_velocity.twist.linear.x += temporary;
	}
	temporary= 1 *( data_velocity.twist.linear.x * sin(adding_yaw)
										+ data_velocity.twist.linear.y * cos(adding_yaw)
									);
	std::cout << "temporary of y is " << std::dec << temporary << "\n";
	if( fabs(temporary) < 0.5 && fabs(temporary) >= 0.01) 
		message_velocity.twist.linear.y += temporary;
	message_velocity.twist.linear.z = data_velocity.twist.linear.z;
	message_velocity.twist.angular.x = data_velocity.twist.angular.x;
	message_velocity.twist.angular.y = data_velocity.twist.angular.y;
	message_velocity.twist.angular.z = -1* data_velocity.twist.angular.z;
}

int main( int argv , char** argc){

	ros::init( argv , argc , "filter_imu_node");

	ros::NodeHandle nh("");
	ros::NodeHandle ph("~");

	std::string name_node = ros::this_node::getName();
	std::string test_param;
	#ifdef TEST_ROS_SYSTEM
		if( ph.getParam("filter_imu/test_param", test_param)){
			std::cout << "<--SYSTEM-->" << name_node << " have get param" << "\n";
		}
		else{
			std::cout << "<--SYSTEM-->" << name_node << " don't have get param" << "\n";
		}
	#endif	
	ph.param("filter_imu/adding_yaw" , adding_yaw , 0.0);
	ph.param("filter_imu/adding_roll" , adding_roll , 0.0);
	ph.param("filter_imu/adding_pitch" , adding_pitch , 0.0);
	ph.param("filter_imu/max_data_of_velocity" , max_data_of_velocity , 10);
	ph.param("filter_imu/max_data_of_euler" , max_data_of_euler , 10);
	ph.param("filter_imu/error_velocity" , error_velocity , 0.008);
	std::string topic_sub_velocity;	
	ph.param("filter_imu/topic_sub_velocity" 
					, topic_sub_velocity , std::string("port_imu/velocity"));
	ros::Subscriber sub_velocity = nh.subscribe( topic_sub_velocity 
					, max_data_of_velocity , &listen_velocity);
	std::string topic_sub_euler;	
	ph.param("filter_imu/topic_sub_euler" , topic_sub_euler , std::string("port_imu/euler"));
	ros::Subscriber sub_euler = nh.subscribe( topic_sub_euler 
					, max_data_of_euler , &listen_euler);
	int frequency;
	ph.param("filter_imu/frequency" , frequency , 50);
	std::string topic_pub_velocity;
	ph.param("filter_imu/topic_pub_velocity" 
					, topic_pub_velocity , std::string("filter_imu/velocity"));
	ros::Publisher tell_velocity = nh.advertise<geometry_msgs::TwistStamped>(
										topic_pub_velocity, 1
									);
	std::string topic_pub_euler;
	ph.param("filter_imu/topic_pub_euler" , topic_pub_euler , std::string("filter_imu/euler"));	
	ros::Publisher tell_euler = nh.advertise<geometry_msgs::Vector3Stamped>(
										topic_pub_euler, 1
									);
//	ph.param("filter_imu/type_filter_velocity" , type_filter_velocity , std::string("max"));
//	ph.param("filter_imu/type_filter_euler" , type_filter_euler , std::string("average")); 

	ros::Rate rate = frequency;
	message_velocity.twist.linear.x = 0;	
	message_velocity.twist.linear.y = 0;
	while( nh.ok() ){
		current_data_velocity = 0;
		current_data_euler = 0;
		ros::spinOnce();
		message_euler.header.stamp = ros::Time::now();	
		message_velocity.header.stamp = ros::Time::now();
		set_meesage_velocity();	
		tell_velocity.publish( message_velocity );
		tell_euler.publish( message_euler );
		rate.sleep();	
	}
}
