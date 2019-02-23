/*
	File name			:	error_code.cpp
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 20
	Date last modified	:	2018 , ??? ??
	Purpose				:	Library for try to tell you what error what happend in zeabus_library
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Using

	Namespace			:	zeabus_library
*/

#include	<zeabus_library/error_code.h>

namespace zeabus_library{

	void print_error( std::string message ){
		printf( NRE "ERROR DETECTION %s\n" RES , message.c_str() );
	}

	void print_warning( std::string message ){
		printf( NYE "WARNING! %s\n" RES , message.c_str() );
	}

}
