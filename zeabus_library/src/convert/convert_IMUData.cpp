/*
	File name			:	convert_IMUData.cpp	
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

#include	<zeabus_library/convert_IMUData.h>

namespace zeabus_library{

	void array_to_IMUData( double* set_value , zeabus_library::IMUData& result, int offset ){
		array_to_Point3( set_value , result.euler , offset + 0 );
		array_to_Point3( set_value , result.angular_velocity , offset + 3 );
		array_to_Point3( set_value , result.linear_acceleration , offset + 6 );
	}

	void split_array_to_IMUData( double* euler , double* angular_velocity 
								, double* linear_acceleration
								, zeabus_library::IMUData& result ){
		array_to_Point3(euler , result.euler , 0 );
		array_to_Point3(angular_velocity , result.angular_velocity , 0 );
		array_to_Point3(linear_acceleration , result.linear_acceleration , 0 );
	}

	void matrix_to_IMUData(	boost::numeric::ublas::matrix< double > set_value 
							, zeabus_library::IMUData& result , int offset_row ){
		matrix_to_Point3( set_value , result.euler , offset_row + 0 );
		matrix_to_Point3( set_value , result.angular_velocity , offset_row + 1 );
		matrix_to_Point3( set_value , result.linear_acceleration , offset_row + 2 );
	}

	void split_matrix_to_IMUData(	boost::numeric::ublas::matrix< double > euler
									, boost::numeric::ublas::matrix< double > angular_velocity
									, boost::numeric::ublas::matrix< double > linear_acceleration
									, zeabus_library::IMUData& result ){
		matrix_to_Point3( euler , result.euler , 0 );	
		matrix_to_Point3( angular_velocity , result.angular_velocity , 0 );	
		matrix_to_Point3( linear_acceleration , result.linear_acceleration , 0 );	
	}

}

