/*
	File name			:	general.cpp	
	Author				:	Supasan Komonlit
	Date created		:	2018 , JAN 20
	Date last modified	:	2018 , ??? ??
	Purpose				:	source code of genral function or ulity function
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	product

	Namespace			:	zeabus_library
*/

#include	<zeabus_library/general.h>

namespace zeabus_library{

	bool check_zero( double number ){
		if( number > EPSILON ) return false;
		else if( number < EPSILON ) return false;
		else return true;
	}

	bool check_zero( float number ){
		if( number > EPSILON ) return false;
		else if( number < EPSILON ) return false;
		else return true;
	}

	bool check_zero( int number ){
		if( number > EPSILON ) return false;
		else if( number < EPSILON ) return false;
		else return true;
	}

}
