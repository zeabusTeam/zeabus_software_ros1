/*
	File name			:	TF_data.h		
	Author				:	Supasan Komonlit
	Date created		:	2019 , FEB 01
	Date last modified	:	2019 , ??? ??
	Purpose				:	
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Maintain

	Namespace			:	zeabus_static::convert
*/

#include	<tf/tf.h>

#include	<stdio.h>

#include	<iostream>

#include	<geometry_msgs/Vector3.h>

#include	<geometry_msgs/Quaternion.h>

#ifndef _ZEABUS_STATIC_CONVERT_TF_DATA_H__
#define _ZEABUS_STATIC_CONVERT_TF_DATA_H__

namespace zeabus_static{

namespace convert{

	tf::Vector3 tf_Vector3( geometry_msgs::Vector3& data );

	tf::Quaternion tf_Quaternion( geometry_msgs::Quaternion& data);

}

}

#endif
