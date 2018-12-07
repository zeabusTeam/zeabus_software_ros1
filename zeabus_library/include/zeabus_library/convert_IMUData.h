/*
	File name			:	convert_IMUData.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 07
	Date last modified	:	2018 , DEC 07
	Purpose				:	For convert value to zeabus_library/IMUData
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Maintain

	Namespace			:	zeabus_library
*/

#include	<stdio.h>

#include	<boost/numeric/ublas/matrix.hpp>

#include	<zeabus_library/convert_Point3.h>
#include	<zeabus_library/IMUData.h>

#ifndef _ZEABUS_LIBRARY_CONVERT_IMUData_H__
#define _ZEABUS_LIBRARY_CONVERT_IMUData_H__

namespace zeabus_library{

	void array_to_IMUData( double* set_value , zeabus_library::IMUData& result, int offset = 0);

	void split_array_to_IMUData( double* euler , double* angular_velocity 
								, double* linear_acceleration
								, zeabus_library::IMUData& result );

	void matrix_to_IMUData( boost::numeric::ublas::matrix< double > set_value 
								, zeabus_library::IMUData& result , int offset_row = 0 );

	void split_matrix_to_IMUData(	boost::numeric::ublas::matrix< double > euler
									, boost::numeric::ublas::matrix< double > angular_velocity
									, boost::numeric::ublas::matrix< double > linear_acceleration
									, zeabus_library::IMUData& result );

}

#endif
