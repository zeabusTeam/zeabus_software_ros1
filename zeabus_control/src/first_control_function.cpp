/*
	File name			:	first_control_function.cpp
	Author				:	Supasan Komonlit
	Date created		:	2018 , JAN 20
	Date last modified	:	2018 , ??? ??
	Purpose				:	For helping function
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Production

	Namespace			:	-
*/

#include	<stdio.h>

#include	<iostream>

#include	<zeabus_library/Point3.h>

#include	<zeabus_library/Point4.h>

void clear_point4( zeabus_library::Point4& variable ){
	variable.w = 0;
	variable.x = 0;
	variable.y = 0;
	variable.z = 0;
}

void clear_point3( zeabus_library::Point3& variable ){
	variable.x = 0;
	variable.y = 0;
	variable.z = 0;
}
