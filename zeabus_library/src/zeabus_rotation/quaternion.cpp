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

namespace zeabus_library{

namespace zeabus_rotation{

	Quaternion::Quaternion(){
		this->matrix.resize( 4 , 1 );
		this->inverse_matrix.resize( 4 , 1 );
		this->w = &( this->matrix( 0 , 0 ) );
		this->x = &( this->matrix( 1 , 0 ) );
		this->y = &( this->matrix( 2 , 0 ) );
		this->z = &( this->matrix( 3 , 0 ) );
		#ifdef _DEBUG_ZEABUS_LIBRARY_QUATERNION_VARIABLE_
			printf("Finish setup quaternion handle\n");
		#endif
		this->updated = 0;
	}

	void Quaternion::get_RPY( double& roll , double& pitch , double& yaw ){
		#ifdef _DEBUG_ZEABUS_LIBRARY_QUATERNION_HANDLE_
			zeabus_library::matrix::print( "Matrix Quaternion " , this->matrix );
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

	void Quaternion::normalization(){
		zeabus_library::vector::normalization( this->matrix);
	}

	boost::numeric::ublas::matrix< double > Quaternion::inverse(){
		if( this->updated ) return this->inverse_matrix;
		this->update_inverse();
		return this->inverse_matrix;
	}

	size_t Quaternion::inverse( boost::numeric::ublas::matrix< double >& matrix_result ){
		if( matrix_result.size1() != 4 || matrix_result.size2() != 1 ){
			zeabus_library::print_error( 
				"zeabus_library::zeabus_rotation::Quaternion::inverse matrix_result wrong size");
			return zeabus_library::ERROR_SIZE_MATRIX;
		}
		if( this->updated ){}
		else this->update_inverse();
		matrix_result( 0 , 0 ) = this->inverse_matrix( 0 , 0 );	
		matrix_result( 1 , 0 ) = this->inverse_matrix( 1 , 0 );	
		matrix_result( 2 , 0 ) = this->inverse_matrix( 2 , 0 );	
		matrix_result( 3 , 0 ) = this->inverse_matrix( 3 , 0 );	
		return zeabus_library::NO_ERROR;
	}

	void Quaternion::update_inverse(){
		if( this->updated ) return;
		this->updated = 1;
		zeabus_library::vector::conjugate( this->matrix , this->inverse_matrix );
		zeabus_library::vector::normalization( this->inverse_matrix );
	}

	void Quaternion::set_quaternion( double roll , double pitch , double yaw ){
		this->updated = 0;
		// use sequence rotation by order ZYX < plan >
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
		this->updated = 0;
		this->matrix( 0 , 0 ) = matrix( 0 , 0 );
		this->matrix( 1 , 0 ) = matrix( 1 , 0 );
		this->matrix( 2 , 0 ) = matrix( 2 , 0 );
		this->matrix( 3 , 0 ) = matrix( 3 , 0 );
	}

	void Quaternion::set_quaternion( zeabus_library::Point4 data ){
		this->updated = 0;
		this->matrix( 0 , 0 ) = data.w;
		this->matrix( 1 , 0 ) = data.x;
		this->matrix( 2 , 0 ) = data.y;
		this->matrix( 3 , 0 ) = data.z;
	}

	void Quaternion::set_quaternion( double w , double x , double y , double z ){
		this->updated = 0;
		this->matrix( 0 , 0 ) = w;
		this->matrix( 1 , 0 ) = x;
		this->matrix( 2 , 0 ) = y;
		this->matrix( 3 , 0 ) = z;
	}

}

}
