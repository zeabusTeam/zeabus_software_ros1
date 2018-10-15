/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File's name		: find_error_position.h
//
//	Create on		: Oct 15 , 2018
//	Author			: Supasan Komonlit
//
//	Main purpose	: for using in control part
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _zeabus_extension_find_error_poition_h__

#ifndef IOSTREAM
	#include	<iostream>
	#define IOSTREAM
#endif

#ifndef MATH_H
	#include	<math.h>
	#define		MATH_H
#endif

#ifndef PI
	#define PI 3.14159265
#endif

#ifndef STDIO_H
	#include	<stdio.h>
	#define STDIO_H
#endif

#ifndef STDLIB_H
	#include	<stdlib.h>
	#define STDLIB_H
#endif

namespace zeabus_extension{

namespace zeabus_control{

	template<typename number_type> void convert_angular( number_type& point);

	template<typename number_type> number_type find_distance( int8_t size
															, number_type* point_1
															, number_type* point_2 );

	template<typename number_type> void find_min_angular( number_type start 
												, number_type target , number_type& result);

	template<typename number_type> void find_error_position( number_type *point_1 
					, number_type *point_2 , number_type *result_type );

}

}

#endif
