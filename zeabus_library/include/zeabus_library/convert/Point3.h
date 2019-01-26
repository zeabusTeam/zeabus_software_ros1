/*
	File name			:	Point3.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 06
	Date last modified	:	2018 , DEC 26
	Purpose				:	For convert array or value to zeabus_library/Point3
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.2.0
	status				:	Maintain

	Namespace			:	zeabus_library/convert
*/

#include	<stdio.h>

#include	<boost/numeric/ublas/matrix.hpp>

#include	<zeabus_library/error_code.h>

#include	<zeabus_library/Point3.h>

#ifndef _ZEABUS_LIBRARY_CONVERT_POINT3_H__
#define _ZEABUS_LIBRARY_CONVERT_POINT3_H__

namespace zeabus_library{
namespace convert{

	void array_to_Point3( double* set_value , zeabus_library::Point3& result , int offset = 0 );

	// can use only matrix size ( 1 , 3 )	
	size_t matrix_to_Point3(	boost::numeric::ublas::matrix< double >& set_value 
							, zeabus_library::Point3& result , int offset_row = 0 );

	size_t Point3_to_matrix(	zeabus_library::Point3& data
							, boost::numeric::ublas::matrix< double >& result );

	void Point3_to_array(	zeabus_library::Point3& data , double* result , int offset = 0 );

}
}

#endif
