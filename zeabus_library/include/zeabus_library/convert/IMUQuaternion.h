/*
	File name			:	IMUQuaternion.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 23
	Date last modified	:	2018 , DEC 26
	Purpose				:	For convert value to zeabus_library/IMUData
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.1.0
	status				:	Maintain

	Namespace			:	zeabus_library
*/

#include	<stdio.h>

#include	<boost/numeric/ublas/matrix.hpp>

#include	<zeabus_library/convert/Point3.h>

#include	<zeabus_library/convert/Point4.h>

#include	<zeabus_library/IMUQuaternion.h>

#ifndef _ZEABUS_LIBRARY_CONVERT_IMUQUATERNION_H__
#define _ZEABUS_LIBRARY_CONVERT_IMUQUATERNION_H__

namespace zeabus_library{
namespace convert{

	void array_to_IMUQuaternion( double* set_value , zeabus_library::IMUQuaternion& result
							, int offset = 0);

	void split_array_to_IMUQuaternion( double* quaternion , double* angular_velocity 
								, double* linear_acceleration
								, zeabus_library::IMUQuaternion& result );

	void matrix_to_IMUQuaternion( boost::numeric::ublas::matrix< double >& set_value 
								, zeabus_library::IMUQuaternion& result , int offset_row = 0 );

	void split_matrix_to_IMUQuaternion(	boost::numeric::ublas::matrix< double >& euler
									, boost::numeric::ublas::matrix< double >& angular_velocity
									, boost::numeric::ublas::matrix< double >& linear_acceleration
									, zeabus_library::IMUQuaternion& result );

}
}

#endif
