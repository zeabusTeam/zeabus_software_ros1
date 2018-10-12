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
#define TEST_ROS_SYSTEM
#define PI  3.141592653589793;

double adding_yaw;
double adding_roll;
double adding_pitch;
int max_data_of_velocity;
int max_data_of_euler;
int amont_data_of_velocity;
int amont_data_of_euler;
geometry_msgs::TwistStamped* data_of_velocity;
geometry_msgs::Vector3Stamped* data_of_euler;

void bound_range_radian( float& radian){
	for( ; radian < 0 && radian > 6.28 ; ){
		if( radian < 0 ) radian += 6.28;
		else if( radian > 6.28 ) radian -= 6.28;
	}
}


int main( int argv , char** argc){

	ros::init("filter_imu_node");

	ros::NodeHandle nh("");
	ros::NodeHandle ph("~");

	std::string name_node = ros::this_node::getName();
	#ifdef TEST_ROS_SYSTEM
		if( ph.getParam("filter_imu/name_service_status", imu_name_service_status)){
			std::cout << "<--SYSTEM-->" << name_node << " have get param" << "\n";
		}
		else{
			std::cout << "<--SYSTEM-->" << name_node << " don't have get param" << "\n";
		}
	#endif	
	ph.getParam("filter_imu/adding_yaw" , adding_yaw , 0);
	ph.getParam("filter_imu/adding_roll" , adding_roll , 0);
	ph.getParam("filter_imu/adding_pitch" , adding_pitch , 0);
	ph.getParam("filter_imu/max_data_of_velocity" , max_data_of_velocity , 10);
	amont_data_of_velocity = max_data_of_velocity;
	ph.getParam("filter_imu/max_data_of_euler" , max_data_of_euler , 10);
	amont_data_of_euler = max_data_of_euler;
	data_of_velocity = malloc( sizeof( geometry_msgs::TwistStamped ) * amont_data_of_velocity);
	data_of_euler = malloc( sizeof( geometry_msgs::Vector3Stamped )  * amont_data_of_euler);
	

}


