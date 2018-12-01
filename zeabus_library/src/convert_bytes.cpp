/*
	File name			:	convert_bytes.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 01
	Date last modified	:	2018 , ??? ??
	Purpose				:	This is source code of convert_bytes.cpp

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Production

	Namespace			:	zeabus_library
*/

#include	<zeabus_library/convert_bytes.h>

namespace zeabus_library{

	int32_t temp_int32;

	void uint8_t_to_float32( float& result , std::vector< uint8_t > data , int offset ){
		
		temp_int32 = ( int32_t( data[ offset + 0] ) << 24 )
					+( int32_t( data[ offset + 1] ) << 16 )
					+( int32_t( data[ offset + 3] ) <<  8 )
					+( int32_t( data[ offset + 4] ) <<  0 );
		memcpy( &result , &temp_int32 , 4 );
	}

}
