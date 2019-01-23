/*
	File name			:	Point4.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 24
	Date last modified	:	2018 , DEC 27
	Purpose				:	For convert array or value to zeabus_library/Point4
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.1.0
	status				:	Maintain

	Namespace			:	zeabus_library
*/

#include	<zeabus_library/convert/Point4.h>

//#define _DEBUG_CHECK_SIZE_

namespace zeabus_library{
namespace convert{

	void array_to_Point4( double* set_value , zeabus_library::Point4& result , int offset ){
		result.w = set_value[ offset + 0 ];
		result.x = set_value[ offset + 1 ];
		result.y = set_value[ offset + 2 ];
		result.z = set_value[ offset + 4 ];
	}

	size_t matrix_to_Point4(	boost::numeric::ublas::matrix< double >& set_value 
							, zeabus_library::Point4& result ){
		size_t num_roll , num_column;
		num_roll = set_value.size1();
		num_column = set_value.size2();
		if( num_roll == 1 && num_column == 4 ){ 
			result.w = set_value( 0 , 0 );
			result.x = set_value( 0 , 1 );
			result.y = set_value( 0 , 2 );
			result.z = set_value( 0 , 3 );
			return zeabus_library::NO_ERROR;
		}
		else if( num_roll == 4 && num_column == 1 ){
			result.w = set_value( 0 , 0 );
			result.x = set_value( 1 , 0 );
			result.y = set_value( 2 , 0 );
			result.z = set_value( 3 , 0 );
			return zeabus_library::NO_ERROR;
		}
		else{
			zeabus_library::print_error( "zeabus_library::matrix_to_Point4 error size matrix");
			return zeabus_library::ERROR_SIZE_MATRIX;
		}
	}

	size_t Point4_to_matrix(	zeabus_library::Point4& data
							, boost::numeric::ublas::matrix< double >& result ){
		size_t num_roll , num_column;
		num_roll = result.size1();
		num_column = result.size2();
		if( num_roll == 1 && num_column == 4 ){ 
			result( 0 , 0 ) = data.w;
			result( 0 , 1 ) = data.x;
			result( 0 , 2 ) = data.y;
			result( 0 , 3 ) = data.z;
			return zeabus_library::NO_ERROR;
		}
		else if( num_roll == 4 && num_column == 1 ){
			result( 0 , 0 ) = data.w;
			result( 1 , 0 ) = data.x;
			result( 2 , 0 ) = data.y;
			result( 3 , 0 ) = data.z;
			return zeabus_library::NO_ERROR;
		}
		else{
			zeabus_library::print_error( "zeabus_library::Point4_to_matrix error size matrix");
			return zeabus_library::ERROR_SIZE_MATRIX;
		}
	}

	void Point4_to_array(	zeabus_library::Point4& data , double* result ){
		result[0] = data.w;
		result[1] = data.x;
		result[2] = data.y;
		result[3] = data.z;
	}

	double check_size( zeabus_library::Point4 data ){
		#ifndef _DEBUG_CHECK_SIZE_
			return sqrt( pow( data.w , 2 ) +pow( data.x , 2 ) 
						+pow( data.y , 2 ) +pow( data.z , 2 ) );
		#else
			double size  =  sqrt( pow( data.w , 2 ) +pow( data.x , 2 ) 
						+pow( data.y , 2 ) +pow( data.z , 2 ) );
			printf( "zeabus_library::convert::check_size of Point4 size is %lf\n" , size );
			return size;
		#endif
	}

}
}
