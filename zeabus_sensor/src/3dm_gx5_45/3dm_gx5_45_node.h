/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File	: 3dm_gx5_45_node.cpp
//
//	Create	: Oct 07 , 2018
//	Author	: Supasan Komonlit
//
//	Main Purpose : Header of 3dm_gx5_45_node.cpp
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ZEABUS_EXTENSION_MANAGE_PORT_H
	#include	<zeabus_extension/manage_port.h>
	#define	ZEABUS_EXTENSION_MANAGE_PORT_H
#endif

#ifndef IOSTREAM
	#include	<iostream>
	#define	IOSTREAM
#endif

#ifndef ROS_ROS_H
	#include	<ros/ros.h>
	#define ROS_ROS_H
#endif

#ifndef ZEABUS_EXTENSION_CONVERT_TO_STRING_H
	#include	<zeabus_extension/convert_to_string.h>
	#define ZEABUS_EXTENSION_CONVERT_TO_STRING_H
#endif

zeabus_extension::manage_port::specific_port* imu_port;
