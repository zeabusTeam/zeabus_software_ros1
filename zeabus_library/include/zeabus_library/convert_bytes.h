/*
	File name			:	convert_bytes.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 01
	Date last modified	:	2018 , ??? ??
	Purpose				:	This is header of file of convert for main purpose is using in IMU
							can use only situation 4 bytes ( 32-bit)
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.1.0
	status				:	Maintain & Using

	Namespace			:	zeabus_library
*/

#include	<stdio.h> // include standard library of c
#include	<iostream> // include standard library of cpp
#include	<stdint.h> //  import type uint8_t

#include	<string.h> // This is have to include because we will use function in this header
#include	<vector> // for make comfortable we will receive vector for you

#include	<zeabus_library/Point3.h>
#include	<geometry_msgs/Quaternion.h>
#include	<geometry_msgs/Vector3.h>

#ifndef _ZEABUS_LIBRRY_CONVERT_BYTES_H__
#define _ZEABUS_LIBRRY_CONVERT_BYTES_H__

	namespace zeabus_library{

		void uint8_t_to_float32( float& result , std::vector< uint8_t >& data , int offset = 0);

		void uint8_t_to_double64( double& result , std::vector< uint8_t >& data , int offset = 0);

		void uint8_t_to_Point3( zeabus_library::Point3& result 
								, std::vector< uint8_t >& data 
								, int offset );

		void uint8_t_to_Quaternion( geometry_msgs::Quaternion& result 
								, std::vector< uint8_t >& data
								, int offset );

		void uint8_t_to_Vector3( geometry_msgs::Vector3& result 
								, std::vector< uint8_t >& data
								, int offest );

	}

#endif
