/*
	File name			:	general.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , JAN 20
	Date last modified	:	2018 , ??? ??
	Purpose				:	This is header of general function

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	production

	Namespace			:	zeabus_library
*/

#include	<stdio.h>

#include	<iostream>

#include	<math.h>

#ifndef PI
	#define	PI 3.14159265
#endif

#ifndef EPSILON
	#define	EPSILON 1.0e-7
#endif

#ifndef _ZEABUS_LIBRARY_GENERAL_H__
#define _ZEABUS_LIBRARY_GENERAL_H__

namespace zeabus_library{

	bool check_zero( double number );
	bool check_zero( float number );
	bool check_zero( int number );

	bool check_equal( double number_01 , double number_02 );
	bool check_equal( int number_01 , int number_02 );

	double abs( double number );
	int abs( int abs );


}

#endif
