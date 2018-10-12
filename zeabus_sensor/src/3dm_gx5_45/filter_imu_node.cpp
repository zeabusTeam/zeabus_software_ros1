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
int current_data_velocity;
int current_data_euler;
std::string type_filter_velocity;
std::string type_filter_euler;
geometry_msgs::TwistStamped* data_of_velocity;
geometry_msgs::Vector3Stamped* data_of_euler;
geometry_msgs::TwistStamped message_velocity;
geometry_msgs::Vector3Stamped message_euler;

double absolute( double number){
	if( number < 0 ) return -1*number;
	else return number;
}

template< typename type > type choose_max( type result , type target){
	if( absolute(result) > absolute(target) ){
		return result;
	}
	else{
		return target;
	}
}

void bound_range_radian( float& radian){
	for( ; radian < 0 && radian > 6.28 ; ){
		if( radian < 0 ) radian += 6.28;
		else if( radian > 6.28 ) radian -= 6.28;
	}
}

void listen_velocity( const geometry_msgs::TwistStamped message){
	data_of_velocity[ current_data_velocity ] = message;
	current_data_velocity++;
}

void listen_euler( const geometry_msgs::Vector3Stamped message){
	data_of_euler[current_data_euler] = message;
	current_data_euler++;
}

void reset_velocity( geometry_msgs::TwistStamped& message_velocity){
	message_velocity.twist.linear.x = 0;
	message_velocity.twist.linear.y = 0;
	message_velocity.twist.linear.z = 0;
	message_velocity.twist.angular.x = 0;
	message_velocity.twist.angular.y = 0;
	message_velocity.twist.angular.z = 0;
}

void filter_euler(){
	std::cout << "current_euler " << current_data_euler << "\n";
	if( type_filter_euler == "average"){
		for( int run = 0 ; run < current_data_euler ; run++){
			if( run == 0 ){
				message_euler.vector.x = data_of_euler[run].vector.x;
				message_euler.vector.y = data_of_euler[run].vector.y;
				message_euler.vector.z = data_of_euler[run].vector.z;
			}else{
				message_euler.vector.x += data_of_euler[run].vector.x;
				message_euler.vector.y += data_of_euler[run].vector.y;
				message_euler.vector.z += data_of_euler[run].vector.z;
			}
			message_euler.vector.x /= current_data_euler;
			message_euler.vector.y /= current_data_euler;
			message_euler.vector.z /= current_data_euler;
		}

	}else if( type_filter_euler == "max"){
		for( int run = 0 ; run < current_data_euler ; run++){
			message_euler.vector.x = 
				choose_max( message_euler.vector.x , data_of_euler[run].vector.x);
			message_euler.vector.y = 
				choose_max( message_euler.vector.y , data_of_euler[run].vector.y);
			message_euler.vector.z = 
				choose_max( message_euler.vector.z , data_of_euler[run].vector.z);
		}
	}else if( type_filter_euler == "min"){

	}else{
		std::cout << "don't have this type" << "\n";
	}		
}

void filter_velocity(){
	std::cout << "current_velocity " << current_data_velocity << "\n";
	if( type_filter_velocity == "average"){
		reset_velocity( message_velocity );
		for(int run = 0 ; run < current_data_velocity ; run++){
			message_velocity.twist.linear.x += data_of_velocity[run].twist.linear.x;	
			message_velocity.twist.linear.y += data_of_velocity[run].twist.linear.y;	
			message_velocity.twist.linear.z += data_of_velocity[run].twist.linear.z;	
			message_velocity.twist.angular.x += data_of_velocity[run].twist.angular.x;	
			message_velocity.twist.angular.y += data_of_velocity[run].twist.angular.y;	
			message_velocity.twist.angular.z += data_of_velocity[run].twist.angular.z;	
		}
		message_velocity.twist.linear.x /= current_data_velocity;	
		message_velocity.twist.linear.y /= current_data_velocity;	
		message_velocity.twist.linear.z /= current_data_velocity;	
		message_velocity.twist.angular.x /= current_data_velocity;
		message_velocity.twist.angular.y /= current_data_velocity;	
		message_velocity.twist.angular.z /= current_data_velocity;
	}else if( type_filter_velocity == "max"){
		reset_velocity( message_velocity );
		for( int run = 0 ; run < current_data_velocity ; run++){
			message_velocity.twist.linear.x =
				choose_max( message_velocity.twist.linear.x 
							, data_of_velocity[run].twist.linear.x);
			message_velocity.twist.linear.y =
				choose_max( message_velocity.twist.linear.y 
							, data_of_velocity[run].twist.linear.y);
			message_velocity.twist.linear.z =
				choose_max( message_velocity.twist.linear.z 
							, data_of_velocity[run].twist.linear.z);
			message_velocity.twist.angular.x =
				choose_max( message_velocity.twist.angular.x 
							, data_of_velocity[run].twist.angular.x);
			message_velocity.twist.angular.y =
				choose_max( message_velocity.twist.angular.y 
							, data_of_velocity[run].twist.angular.y);
			message_velocity.twist.angular.z =
				choose_max( message_velocity.twist.angular.z 
							, data_of_velocity[run].twist.angular.z);
		}

	}else if( type_filter_velocity == "min"){

	}else{
		std::cout << "don't have this type" << "\n";
	}		

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
	std::string topic_sub_velocity;	
	ph.param("filter_imu/topic_sub_velocity" , topic_sub_velocity , std::string("port_imu/velocity"));
	ros::Subscriber sub_velocity = nh.subscribe( topic_sub_velocity , max_data_of_velocity , &listen_velocity);
	std::string topic_sub_euler;	
	ph.param("filter_imu/topic_sub_euler" , topic_sub_euler , std::string("port_imu/euler"));
	ros::Subscriber sub_euler = nh.subscribe( topic_sub_euler , max_data_of_euler , &listen_euler);
	int frequency;
	ph.param("filter_imu/frequency" , frequency , 50);
	data_of_velocity = new geometry_msgs::TwistStamped[max_data_of_velocity];
	data_of_euler = new geometry_msgs::Vector3Stamped[max_data_of_euler];
	std::string topic_pub_velocity;
	ph.param("filter_imu/topic_pub_velocity" , topic_pub_velocity , std::string("filter_imu/velocity"));
	ros::Publisher tell_velocity = nh.advertise<geometry_msgs::TwistStamped>(
										topic_pub_velocity, 1
									);
	std::string topic_pub_euler;
	ph.param("filter_imu/topic_pub_euler" , topic_pub_euler , std::string("filter_imu/euler"));	
	ros::Publisher tell_euler = nh.advertise<geometry_msgs::Vector3Stamped>(
										topic_pub_euler, 1
									);
	ph.param("filter_imu/type_filter_velocity" , type_filter_velocity , std::string("max"));
	ph.param("filter_imu/type_filter_euler" , type_filter_euler , std::string("average")); 

	ros::Rate rate = frequency;
	
	while( nh.ok() ){
		current_data_velocity = 0;
		current_data_euler = 0;
		ros::spinOnce();
		filter_euler();
		filter_velocity();
		message_euler.header.stamp = ros::Time::now();	
		message_velocity.header.stamp = ros::Time::now();	
		tell_velocity.publish( message_velocity );
		tell_euler.publish( message_euler );
		rate.sleep();	
	}
}
