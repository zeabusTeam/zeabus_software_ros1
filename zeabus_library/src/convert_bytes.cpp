/*
	File name			:	convert_bytes.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 01
	Date last modified	:	2018 , DEC 05
	Purpose				:	This is source code of convert_bytes.cpp

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.1.0
	status				:	Maintain & Used

	Namespace			:	zeabus_library
*/

#include	<zeabus_library/convert_bytes.h>

//#define _TEST_CONVERT_

namespace zeabus_library{

	int32_t temp_int32;
	float temp_float32;
	double temp_double64;

	void uint8_t_to_float32( float& result , std::vector< uint8_t >& data , int offset ){
		#ifdef _TEST_CONVERT_
			printf( "value byte is %2X %2X %2X %2X " , data[offset] , data[offset+1]
													, data[offset+2] , data[offset+3]);	
		#endif
		temp_int32 = ( int32_t( data[ offset + 0] ) << 24 )
					+( int32_t( data[ offset + 1] ) << 16 )
					+( int32_t( data[ offset + 2] ) <<  8 )
					+( int32_t( data[ offset + 3] ) <<  0 );
		memcpy( &result , &temp_int32 , 4 );
		#ifdef _TEST_CONVERT_
			printf( "Covert to %X Result is %8.4f\n" , temp_int32 , result );
		#endif
	}

	void uint8_t_to_double64( double& result , std::vector< uint8_t >& data , int offset ){
	
		temp_int32 = ( int32_t( data[ offset + 0] ) << 24 )
					+( int32_t( data[ offset + 1] ) << 16 )
					+( int32_t( data[ offset + 2] ) <<  8 )
					+( int32_t( data[ offset + 3] ) <<  0 );
		memcpy( &temp_float32 , &temp_int32 , 4 );
		result = double( temp_float32 );
	}

	void uint8_t_to_Point3( zeabus_library::Point3& result 
							, std::vector< uint8_t >& data 
							, int offset ){
		uint8_t_to_float32( temp_float32 , data , offset + 0 );
		result.x = temp_float32;
		uint8_t_to_float32( temp_float32 , data , offset + 4 );
		result.y = temp_float32;
		uint8_t_to_float32( temp_float32 , data , offset + 8 );
		result.z = temp_float32;
	}

	void uint8_t_to_Quaternion( geometry_msgs::Quaternion& result 
								, std::vector< uint8_t >& data
								, int offset ){
		uint8_t_to_float32( temp_float32 , data , offset + 0);
		result.w = temp_float32;
		uint8_t_to_float32( temp_float32 , data , offset + 4);
		result.x = temp_float32;
		uint8_t_to_float32( temp_float32 , data , offset + 8);
		result.y = temp_float32;
		uint8_t_to_float32( temp_float32 , data , offset + 12);
		result.z = temp_float32;
	}

	void uint8_t_to_Vector3( geometry_msgs::Vector3& result 
								, std::vector< uint8_t >& data
								, int offest ){
		uint8_t_to_float32( temp_float32 , data , offest + 0 );
		result.x = temp_float32;
		uint8_t_to_float32( temp_float32 , data , offest + 4 );
		result.y = temp_float32;
		uint8_t_to_float32( temp_float32 , data , offest + 8 );
		result.z = temp_float32;
	}
}
