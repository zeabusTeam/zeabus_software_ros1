/*
	File name			:	quaternion_structor.cpp	
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 12
	Date last modified	:	2018 , ??? ??
	Purpose				:	source code of structure quaternion
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	product

	Namespace			:	zeabus_library
*/

#include	<zeabus_library/quaternion_handle.h>

namespace zeabus_library{

	QuaternionVariable::QuaternionVariable(){
		this->matrix.resize( 4 , 1 );
		this->w = &( this->matrix( 0 , 0 ) );
		this->x = &( this->matrix( 1 , 0 ) );
		this->y = &( this->matrix( 2 , 0 ) );
		this->z = &( this->matrix( 3 , 0 ) );
	}

	void QuaternionVariable::set_quaternion( double roll , double pitch , double yaw ){
		this->	
	}

}
