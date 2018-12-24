/*
	File name			:	convert_Point4.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 24
	Date last modified	:	2018 , DEC 24
	Purpose				:	For convert array or value to zeabus_library/Point4
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Maintain

	Namespace			:	zeabus_library
*/

#include	<zeabus_library/convert_Point4.h>

namespace zeabus_library{

	void array_to_Point4( double* set_value , zeabus_library::Point4& result , int offset ){
		result.w = set_value[ offset + 0 ];
		result.x = set_value[ offset + 1 ];
		result.y = set_value[ offset + 2 ];
		result.z = set_value[ offset + 4 ];
	}

	void matrix_to_Point4(	boost::numeric::ublas::matrix< double > set_value 
							, zeabus_library::Point4& result , int offset_row ){
		result.w = set_value( offset_row , 0 );
		result.x = set_value( offset_row , 1 );
		result.y = set_value( offset_row , 2 );
		result.z = set_value( offset_row , 3 );
	}

	void Point4_to_matrix(	zeabus_library::Point4& data
							, boost::numeric::ublas::matrix< double >& result ){
		result( 0 , 0 ) = data.w;
		result( 0 , 1 ) = data.x;
		result( 0 , 2 ) = data.y;
		result( 0 , 3 ) = data.z;
	}

	void Point4_to_array(	zeabus_library::Point4& data , double* result ){
		result[0] = data.w;
		result[1] = data.x;
		result[2] = data.y;
		result[3] = data.z;
	}

}
