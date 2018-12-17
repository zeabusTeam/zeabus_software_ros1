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
		this->temp_yaw.resize( 4 , 1 );
		this->temp_pitch.resize( 4 , 1 );
		this->temp_roll.resize( 4 , 1 );
		this->temp_matrix.resize( 4 , 1 );
	}

	void QuaternionVariable::set_quaternion( double roll , double pitch , double yaw ){
		this->temp_yaw( 0 , 0 ) = cos( zeabus_library::euler::radian_domain( yaw ) / 2 );
		this->temp_yaw( 3 , 0 ) = sin( zeabus_library::euler::radian_domain( yaw ) / 2 );
		this->temp_roll( 0 , 0 ) = cos( zeabus_library::euler::radian_domain( roll ) / 2 );
		this->temp_roll( 1 , 0 ) = sin( zeabus_library::euler::radian_domain( roll ) / 2 );
		this->temp_pitch( 0 , 0 ) = cos( zeabus_library::euler::radian_domain( pitch ) / 2 );
		this->temp_pitch( 2 , 0 ) = sin( zeabus_library::euler::radian_domain( pitch ) / 2 );	
		this->temp_matrix = boost::numeric::ublas::prod(	this->temp_yaw
															, this->temp_pitch );
		this->matrix = boost::numeric::ublas::prod(	this->temp_matrix 
															, this->temp_roll );
	}

	void QuaternionVariable::set_quaternion( boost::numeric::ublas::matrix< double > matrix ){
		this->matrix( 0 , 0 ) = matrix( 0 , 0 );
		this->matrix( 1 , 0 ) = matrix( 1 , 0 );
		this->matrix( 2 , 0 ) = matrix( 2 , 0 );
		this->matrix( 3 , 0 ) = matrix( 3 , 0 );
	}

	void QuaternionVariable::set_quaternion( zeabus_library::Point4 data ){
		this->matrix( 0 , 0 ) = data.w;
		this->matrix( 1 , 0 ) = data.x;
		this->matrix( 2 , 0 ) = data.y;
		this->matrix( 3 , 0 ) = data.z;
	}

	void QuaternionVariable::set_quaternion( double w , double x , double y , double z ){
		this->matrix( 0 , 0 ) = w;
		this->matrix( 1 , 0 ) = x;
		this->matrix( 2 , 0 ) = y;
		this->matrix( 3 , 0 ) = z;
	}

}
