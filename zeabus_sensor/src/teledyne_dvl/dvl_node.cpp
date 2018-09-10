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

	std::cout << "Welcome to read value dvl_data output type PD6 ";

	ros::spin();

}


void listen_port_dvl( const std_msgs::String message){
	std::string data = message.data;

//01// READ PART of SYSTEM ATTITUDE DATA
	if( data.find(":SA") != std::string::npos ){}
	else{}

//02// READ PART of TIMING and SCALING DATA
	if( data.find(":TS") != std::string::npos ){}
	else{}

//03// READ PART of WATER-MASS, INSTRUMENT-REFERENCED VELOCITY DATA
	if( data.find(":WI") != std::string::npos ){}
	else{}

//04// READ PART of BOTTOM-TRACK, INSTRUMENT-REFERENCED VELOCITY DATA
	if( data.find(":BI") != std::string::npos ){}
	else{}

//05// READ PART of WATER-MASS, SHIP-REFERENCED VELOCITY DATA
	if( data.find(":WS") != std::string::npos ){}
	else{}

//06// READ PART of BOTTOM-TRACK, SHIP-REFERENCED VELOCITY DATA
	if( data.find(":BS") != std::string::npos ){}
	else{}

//07//	READ PART of WATER-MASS, EARTH-REFERENCED VELOCITY DATA
	if( data.find(":WE") != std::string::npos ){}
	else{}

//08//	READ PART of BOTTOM_TRACK, EARTH-REFERENCED VELOCITY DATA
	if( data.find(":BE") != std::string::npos ){}
	else{}

//09//	READ PART of WATER-MASS, EARTH-REFERENCED DISTANCE DATA
	if( data.find(":WD") != std::string::npos ){}
	else{}

//10//	READ PART of BOTTOM-TRACK, EARTH-REFERENCED DISTANCE DATA
	if( data.find(":BD") != std::string::npos ){}
	else{}
	if( data.find(":WD") != std::string::npos ){}
	else{}
}
