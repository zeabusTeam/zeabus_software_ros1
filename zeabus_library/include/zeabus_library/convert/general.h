/*
	File name			:	general.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , JAN 16
	Date last modified	:	2018 , ??? 16
	Purpose				:	For convert or edit string
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Maintain

	Namespace			:	zeabus_library/convert
*/

#include	<stdio.h>

#include	<iostream>

#ifndef _ZEABUS_LIBRARY_CONVERT_GENERAL_H__
#define _ZEABUS_LIBRARY_CONVERT_GENERAL_H__

namespace zeabus_library{

namespace convert{

	static char set_syntax[7] = { ' ' , ':' , ',' , '-' , '.' , '_' , ';' };

	std::string edit_string( std::string& problem , std::string key );

	void edit_string( std::string& problem , std::string key , std::string& result );

	bool check_syntax( char data );

	std::string to_string( char data );

	std::string to_string( int data );
}
}

#endif
