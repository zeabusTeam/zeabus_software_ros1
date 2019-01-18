/*
	File name			:	error_code.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 20
	Date last modified	:	2018 , ??? ??
	Purpose				:	This header library for meaning and useing tell error code
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Production

	Namespace			:	zeabus_library
*/

#include	<stdio.h>

#include	<iostream>

#include	<zeabus_library/text_color.h>

#ifndef _ZEABUS_LIBRARY_ERROR_CODE_H__
#define _ZEABUS_LIBRARY_ERROR_CODE_H__

namespace zeabus_library{
	
	static const size_t NO_ERROR = 0;
	static const size_t ERROR_SIZE_ARRAY = 1;  
	static const size_t ERROR_SIZE_MATRIX = 2;
	static const size_t ERROR_STATUS = 3;

	void print_error( std::string message );

}

#endif
