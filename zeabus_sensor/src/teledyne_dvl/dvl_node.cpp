/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File	: dvl_node.cpp
//
//	Edit	: Sep 11 , 2018
//	Author	: Supasan Komonlit
//
//	Thanks	: Mahisorn Dec 22 , 2014
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include	"dvl_node.h"

int main( int argc , char **argv){

// init name node but if you have launch this node will name up to launch file
	ros::init( argc , argv, "node_dvl");

// this is equipment of ros
	ros::NodeHandle nh;

// Set to Subscriber from topic dvl/port for geting value form port
	ros::Subscriber subscriber_dvl_port = 
		nh.subscribe( "dvl/port" , 10 , &listen_port_dvl);

// Set to send data form port after read meaning
	ros::Publisher publisher_dvl_data = ros::NodeHandle().advertise<
											geometry_msgs::Twist >( "dvl/data" , 10);

	ros::spin();

}


