/*
	File name			:	convert_Point3.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 06
	Date last modified	:	2018 , DEC 09
	Purpose				:	For convert array or value to zeabus_library/Point3
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.1
	status				:	Maintain

	Namespace			:	zeabus_library
*/

#include	<zeabus_library/convert_Point3.h>

namespace zeabus_library{

	void array_to_Point3( double* set_value , zeabus_library::Point3& result , int offset ){
		result.x = set_value[ offset + 0 ];
		result.y = set_value[ offset + 1 ];
		result.z = set_value[ offset + 2 ];
	}

	void matrix_to_Point3(	boost::numeric::ublas::matrix< double > set_value 
							, zeabus_library::Point3& result , int offset_row ){
		result.x = set_value( offset_row , 0 );
		result.y = set_value( offset_row , 1 );
		result.z = set_value( offset_row , 2 );
	}

	void Point3_to_matrix(	zeabus_library::Point3 data
							, boost::numeric::ublas::matrix< double > result ){
		result( 0 , 0 ) = data.x;
		result( 0 , 1 ) = data.y;
		result( 0 , 2 ) = data.z;
	}

	void Point3_to_array(	zeabus_library::Point3 data , double* result ){
		result[0] = data.x;
		result[1] = data.y;
		result[2] = data.z;
	}

}
