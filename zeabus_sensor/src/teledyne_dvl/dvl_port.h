/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File	: dvl_port.h
//
//	Edit	: Aug 25 , 2018
//	Author	: Supasan Komonlit
//
//	Thanks	: Mahisorn Dec 22 , 2014
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ROS_ROS
	#include	<ros/ros.h>
	#define	ROS_ROS
#endif

#ifndef ZEABUS_SERIAL_SERIAL
	#include	<zeabus_serial/serial.h>
	#define	ZEABUS_SERIAL_SERIAL
#endif

#ifndef ZEABUS_EXTENSION_CONVERT_TO_STRING
	#include	<zeabus_extension/convert_to_string.h>
	#define ZEABUS_EXTENSION_CONVERT_TO_STRING
#endif

/*
#ifndef	DIAGNOSTIC_UPDATER_DIAGNOSTIC_UPDATER
	#include	<dignostic_updater/diagnostic_updater.h>
	#define DIAGNOSTIC_UPDATER_DIAGNOSTIC_UPDATER
#endif

#ifndef	DIAGNOSTIC_UPDATER_PUBLISHER
	#include	<dignostic_updater/publisher.h>
	#define DIAGNOSTIC_UPDATER_PUBLISHER
#endif
*/

#ifndef STD_MSGS_STRING
	#include	<std_msgs/String.h>
	#define STD_MSGS_STRING
#endif

#ifndef IOSTREAM
	#include	<iostream>
	#define		IOSTREAM
#endif

#ifndef STDIO_H
	#include	<stdio.h>
	#define		STDIO_H
#endif


//------------------ Zeabus software will try to extract code to 2 code
//----------- first is code manage about read port and send to next code
//----------- second is code manage about caculate value from port and send to sensor
/*
boost::mutex g_mutex;
bool	g_status_updated = false;
long	g_status_date = 0;
float	g_status_salinity = 0;
float	g_status_temperature = 0;
float	g_status_depth_of_transducer_face = 0;
float	g_status_speed_of_sound = 0;
int		g_status_num_error = 0;
int		g_status_current_error = 0;
bool	g_status_velocity_ok = false;

float	surface_depth;

void	check_dvl_status( diagnostic_updater::DiagnosticStatusWrapper &stat){

	boost::lock_guard<boost::mutex> guard(g_mutext);

	if( g_status_update){

		g_status_ipdated = false;
	
		if( g_status_velocity_ok ){
			stat.summary( diagnostic_msgs::DiagnosticStatus::OK , "Velocity data is Good");
		}
		else{
			stat.summary( diagnostic_msgs::DiagnosticStatus::WARN , "Velocity data is Bad");
		}

		stat.add("Velocity data status", g_status_velocity_ok);
		stat.add("Time (yyMMddhhmmssff)" , g_status_date);
		stat.add("Salinity (ppt)" , g_status_salinity);
		stat.add("Temperature (C)" , g_status_temperature);
		stat.add("Depth of transducer face (m)", g_status_depth_of_transducer_face);
		stat.add("Speed of sound (m/s)" , g_status_speed_of_sound);
		stat.add("Total error", g_status_num_error);
		stat.add("Current error" , g_status_current_error);

	}

} */
