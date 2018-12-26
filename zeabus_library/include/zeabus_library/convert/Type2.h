/*
	File name			:	Type2.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 07
	Date last modified	:	2018 , DEC 26
	Purpose				:	For convert array or value to zeabus_library/Type2
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.1.0
	status				:	Maintain

	Namespace			:	zeabus_library
*/

#include	<stdio.h>

#include	<boost/numeric/ublas/matrix.hpp>

#include	<zeabus_library/convert/Point3.h>
#include	<zeabus_library/Type2.h>

#ifndef _ZEABUS_LIBRARY_CONVERT_TYPE2_H__
#define _ZEABUS_LIBRARY_CONVERT_TYPE2_H__


namespace zeabus_library{
namespace convert{

	void array_to_Type2( double* set_value , zeabus_library::Type2& result , int offset = 0 );

}
}

#endif
