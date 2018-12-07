/*
	File name			:	convert_Point3.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 06
	Date last modified	:	2018 , ??? ??
	Purpose				:	For convert array or value to zeabus_library/IMUData
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.0.5
	status				:	Production

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
							, zeabus_library::Point3& result ){
		result.x = set_value( 0 , 0 );
		result.y = set_value( 0 , 1 );
		result.z = set_value( 0 , 2 );
	}

}
