/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File	: 3dm_gx5_45_node.h
//
//	Create	: Oct 07 , 2018
//	Author	: Supasan Komonlit
//
//	Main Purpose : Header of 3dm_gx5_45_node.cpp
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef IOSTREAM
	#include	<iostream>
	#define	IOSTREAM
#endif

#ifndef ROS_ROS_H
	#include	<ros/ros.h>
	#define ROS_ROS_H
#endif

#ifndef ZEABUS_SENSOR_IMU_PORT_H
	#include	<zeabus_sensor/imu_port.h>
	#define ZEABUS_SENSOR_IMU_PORT_H
#endif

#ifndef ZEABUS_SENSOR_MIP_COMMUNICATION_H
	#include	<zeabus_sensor/MIP_COMMUNICATION.h>
	#define ZEABUS_SENSOR_MIP_COMMUNICATION_H
#endif

#ifndef STRING_H
	#include	<string.h>
	#define STRING_H
#endif

#include	<zeabus_sensor/port_sensor.h>
#include	<geometry_msgs/TwistStamped.h>
#include	<geometry_msgs/Vector3Stamped.h>

#define standard_gravity_constant 9.80665

bool status_port = true;

bool service_manage_port(
		zeabus_sensor::port_sensor::Request &request,
		zeabus_sensor::port_sensor::Response &response
	){
	if( request.status_port){
		status_port = true;
	}
	else{
		status_port = false;
	}
	response.ok = true;
	return true;
}
