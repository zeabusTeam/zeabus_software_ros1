/*
	File name			:	support_part.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2019 , FEB 06
	Date last modified	:	2019 , FEB 09
	Purpose				:	

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Maintainance

	Namespace			:	None
*/
//====================>

#include	<stdio.h>

#include	<iostream>

#include	<math.h>

#include	<geometry_msgs/Quaternion.h>

namespace zeabus_library{

namespace control{

	void clear_quaternion( geometry_msgs::Quaternion& data ){
		data.x = 0 ;
		data.y = 0 ;
		data.z = 0 ;	
		data.w = 1 ;
	}

	template<typename vector3 >void clear_vector3( vector3& data ){
		data.x = 0;
		data.y = 0;
		data.z = 0;
	}

}

}
