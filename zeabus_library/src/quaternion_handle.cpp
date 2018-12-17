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

namespace zeabus_library{

	void QuaternionHandle::get_RPY( double& roll , double& pitch , double& yaw ){
		roll = atan2( 2 * ( (*w) * (*x) + (*y) * (*z) ) 
					,	pow( (*w) , 2 ) - pow( (*x) , 2 ) 
						- pow( (*y) , 2 ) + pow( (*z) , 2) );
		
		yaw = atan2( 2 * ( (*w) * (*z) + (*x) * (*y) )
					,	pow( (*w) , 2 ) + pow( (*x) , 2 ) 
						- pow( (*y) , 2 ) - pow( (*z) , 2) );
		
		pitch = -1 * asin( 2 * ( ( (*x) * (*z) ) - ( (*w) * (*y) ) ) );	
	}

}
