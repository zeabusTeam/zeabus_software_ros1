/*
	File name			:	quaternion.cpp	
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 20
	Date last modified	:	2018 , ??? ??
	Purpose				:	source code of structure quaternion
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	product

	Namespace			:	zeabus_library
*/

#include	<zeabus_library/zeabus_rotation/quaternion.h>

#define _DEBUG_ZEABUS_LIBRARY_QUATERNION_VARIABLE_

namespace zeabus_library::zeabus_rotation{

	Quaternion::Quaternion(){
		this->matrix.resize( 4 , 1 );
		this->w = &( this->matrix( 0 , 0 ) );
		this->x = &( this->matrix( 1 , 0 ) );
		this->y = &( this->matrix( 2 , 0 ) );
		this->z = &( this->matrix( 3 , 0 ) );
		#ifdef _DEBUG_ZEABUS_LIBRARY_QUATERNION_VARIABLE_
			printf("Finish setup quaternion handle\n");
		#endif
	}

	void Quaternion::set_quaternion( double roll , double pitch , double yaw ){
		// use sequence rotation by order ZYX
		this->cos_yaw = cos( zeabus_library::euler::radian_domain( yaw ) / 2 );
		this->sin_yaw = sin( zeabus_library::euler::radian_domain( yaw ) / 2 );
		this->cos_roll = cos( zeabus_library::euler::radian_domain( roll ) / 2 );
		this->sin_roll = sin( zeabus_library::euler::radian_domain( roll ) / 2 );
		this->cos_pitch = cos( zeabus_library::euler::radian_domain( pitch ) / 2 );
		this->sin_pitch = sin( zeabus_library::euler::radian_domain( pitch ) / 2 );

		*(this->w) = cos_roll*cos_pitch*cos_yaw + sin_roll*sin_pitch*sin_yaw;
		*(this->x) = sin_roll*cos_pitch*cos_yaw - cos_roll*sin_pitch*sin_yaw;
		*(this->y) = cos_roll*sin_pitch*cos_yaw + sin_roll*cos_pitch*sin_yaw;
		*(this->z) = cos_roll*cos_pitch*sin_yaw - sin_roll*sin_pitch*cos_yaw;	
	}

	void Quaternion::set_quaternion( boost::numeric::ublas::matrix< double > matrix ){
		this->matrix( 0 , 0 ) = matrix( 0 , 0 );
		this->matrix( 1 , 0 ) = matrix( 1 , 0 );
		this->matrix( 2 , 0 ) = matrix( 2 , 0 );
		this->matrix( 3 , 0 ) = matrix( 3 , 0 );
	}

	void Quaternion::set_quaternion( zeabus_library::Point4 data ){
		this->matrix( 0 , 0 ) = data.w;
		this->matrix( 1 , 0 ) = data.x;
		this->matrix( 2 , 0 ) = data.y;
		this->matrix( 3 , 0 ) = data.z;
	}

	void Quaternion::set_quaternion( double w , double x , double y , double z ){
		this->matrix( 0 , 0 ) = w;
		this->matrix( 1 , 0 ) = x;
		this->matrix( 2 , 0 ) = y;
		this->matrix( 3 , 0 ) = z;
	}

}
