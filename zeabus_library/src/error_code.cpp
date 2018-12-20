/*
	File name			:	error_code.cpp
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 20
	Date last modified	:	2018 , ??? ??
	Purpose				:	Library for try to tell you what error what happend in zeabus_library
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Production

	Namespace			:	zeabus_library
*/

#include	<zeabus_library/error_code.h>

namespace zeabus_library{

	void print( std::string message ){
		printf( NRE "ERROR DETECTION %s" RES , message );
	}

}
