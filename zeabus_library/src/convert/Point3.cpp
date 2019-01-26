/*
	File name			:	Point3.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 06
	Date last modified	:	2018 , JAN 26
	Purpose				:	For convert array or value to zeabus_library/Point3
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.2.0
	status				:	Maintain

	Namespace			:	zeabus_library
*/

#include	<zeabus_library/convert/Point3.h>

namespace zeabus_library{
namespace convert{

	void array_to_Point3( double* set_value , zeabus_library::Point3& result , int offset ){
		result.x = set_value[ offset + 0 ];
		result.y = set_value[ offset + 1 ];
		result.z = set_value[ offset + 2 ];
	}

	size_t matrix_to_Point3(	boost::numeric::ublas::matrix< double >& set_value 
							, zeabus_library::Point3& result , int offset_row ){
		size_t num_roll , num_column;
		num_roll = set_value.size1();
		num_column = set_value.size2();
		if( num_roll == 1 && num_column == 3 ){ 
			result.x = set_value( 0 , 0 );
			result.y = set_value( 0 , 1 );
			result.z = set_value( 0 , 2 );
			return zeabus_library::NO_ERROR;
		}
		else if( num_roll == 3 && num_column == 1 ){
			result.x = set_value( 0 , 0 );
			result.y = set_value( 1 , 0 );
			result.z = set_value( 2 , 0 );
			return zeabus_library::NO_ERROR;
		}
		else{
			zeabus_library::print_error( "zeabus_library::matrix_to_Point3 error size matrix");
			return zeabus_library::ERROR_SIZE_MATRIX;
		}
	}

	size_t Point3_to_matrix(	zeabus_library::Point3& data
							, boost::numeric::ublas::matrix< double >& result ){
		size_t num_roll , num_column;
		num_roll = result.size1();
		num_column = result.size2();
		if( num_roll == 1 && num_column == 3 ){ 
			result( 0 , 0 ) = data.x;
			result( 0 , 1 ) = data.y;
			result( 0 , 2 ) = data.z;
			return zeabus_library::NO_ERROR;
		}
		else if( num_roll == 3 && num_column == 1 ){
			result( 0 , 0 ) = data.x;
			result( 1 , 0 ) = data.y;
			result( 2 , 0 ) = data.z;
			return zeabus_library::NO_ERROR;
		}
		else{
			zeabus_library::print_error( "zeabus_library::Point3_to_matrix error size matrix");
			return zeabus_library::ERROR_SIZE_MATRIX;
		}
	}

	void Point3_to_array( zeabus_library::Point3& data , double* result , int offset){
		result[0 + offset ] = data.x;
		result[1 + offset ] = data.y;
		result[2 + offset ] = data.z;
	}

}
}
