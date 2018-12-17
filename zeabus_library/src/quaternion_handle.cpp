/*
	File name			:	quaternion_handle.cpp
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 05
	Date last modified	:	2018 , DEC 12
	Purpose				:	This is header of code for use metrix trasform by orientation

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Stop because failure when apply with acceleration data from IMU

	Namespace			:	zeabus_library
*/

#include	<zeabus_library/quaternion_handle.h>

#define _DEBUG_ZEABUS_LIBRARY_QUATERNION_HANDLE_

namespace zeabus_library{

	QuaternionHandle::QuaternionHandle(){
		this->temp_quaternion.resize( 4 , 1 );
	}

	void QuaternionHandle::get_RPY( double& roll , double& pitch , double& yaw ){
		#ifdef _DEBUG_ZEABUS_LIBRARY_QUATERNION_HANDLE_
			this->matrix_handle.print_individual_matrix( "Matrix Quaternion " , this->matrix );
		#endif
		roll = atan2( 2 * ( (*w) * (*x) + (*y) * (*z) ) 
					,	pow( (*w) , 2 ) - pow( (*x) , 2 ) 
						- pow( (*y) , 2 ) + pow( (*z) , 2) );
		
		yaw = atan2( 2 * ( (*w) * (*z) + (*x) * (*y) )
					,	pow( (*w) , 2 ) + pow( (*x) , 2 ) 
						- pow( (*y) , 2 ) - pow( (*z) , 2) );
		
		pitch = -1 * asin( 2 * ( ( (*x) * (*z) ) - ( (*w) * (*y) ) ) );
		#ifdef _DEBUG_ZEABUS_LIBRARY_QUATERNION_HANDLE_
			printf( "Result roll : pitch : yaw %10.6lf : %10.6lf : %10.6lf \n" 
					, roll , pitch ,yaw );
		#endif	
	}

}
