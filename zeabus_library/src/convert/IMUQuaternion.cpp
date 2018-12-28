/*
	File name			:	IMUQuaternion.cpp	
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 23
	Date last modified	:	2018 , DEC 27
	Purpose				:	For convert value to zeabus_library/IMUQuaternion
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.1.0
	status				:	Maintain

	Namespace			:	zeabus_library/convert
*/

#include	<zeabus_library/convert/IMUQuaternion.h>

namespace zeabus_library{
namespace convert{

	void array_to_IMUQuaternion( double* set_value , zeabus_library::IMUQuaternion& result
							, int offset ){
		array_to_Point4( set_value , result.quaternion );
		array_to_Point3( set_value , result.angular_velocity , offset + 4 );
		array_to_Point3( set_value , result.linear_acceleration , offset + 7 );
	}

	void split_array_to_IMUQuaternion( double* quaternion , double* angular_velocity 
								, double* linear_acceleration
								, zeabus_library::IMUQuaternion& result ){
		array_to_Point4(quaternion , result.quaternion );
		array_to_Point3(angular_velocity , result.angular_velocity , 0 );
		array_to_Point3(linear_acceleration , result.linear_acceleration , 0 );
	}

	void matrix_to_IMUQuaternion(	boost::numeric::ublas::matrix< double > set_value 
							, zeabus_library::IMUQuaternion& result , int offset_row ){
		matrix_to_Point4( set_value , result.quaternion );
		matrix_to_Point3( set_value , result.angular_velocity , offset_row + 1 );
		matrix_to_Point3( set_value , result.linear_acceleration , offset_row + 2 );
	}

	void split_matrix_to_IMUQuaternion(	boost::numeric::ublas::matrix< double >& quaternion
									, boost::numeric::ublas::matrix< double >& angular_velocity
									, boost::numeric::ublas::matrix< double >& linear_acceleration
									, zeabus_library::IMUQuaternion& result ){
		matrix_to_Point4( quaternion , result.quaternion );	
		matrix_to_Point3( angular_velocity , result.angular_velocity , 0 );	
		matrix_to_Point3( linear_acceleration , result.linear_acceleration , 0 );	
	}

}
}

