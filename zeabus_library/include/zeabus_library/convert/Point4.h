/*
	File name			:	Point4.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 24
	Date last modified	:	2018 , DEC 26
	Purpose				:	For convert array or value to zeabus_library/Point4
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.1.0
	status				:	Maintain

	Namespace			:	zeabus_library
*/

#include	<stdio.h>

#include	<math.h>

#include	<cstdlib>

#include	<boost/numeric/ublas/matrix.hpp>

#include	<zeabus_library/Point4.h>

#include	<zeabus_library/error_code.h>

#ifndef _ZEABUS_LIBRARY_CONVERT_POINT4_H__
#define _ZEABUS_LIBRARY_CONVERT_POINT4_H__

namespace zeabus_library{
namespace convert{

	void array_to_Point4( double* set_value , zeabus_library::Point4& result , int offset = 0 );

	// can use only matrix size ( 1 , 3 )	
	size_t matrix_to_Point4( boost::numeric::ublas::matrix< double >& set_value 
							, zeabus_library::Point4& result );

	size_t Point4_to_matrix(	zeabus_library::Point4& data
							, boost::numeric::ublas::matrix< double >& result );

	void Point4_to_array(	zeabus_library::Point4& data , double* result);

	double check_size( zeabus_library::Point4 data );

}
}

#endif
