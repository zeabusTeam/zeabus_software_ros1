/*
	File name			:	rotation_handle.cpp
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 23
	Date last modified	:	2018 , ??? ??
	Purpose				:	This is file for apply matrix and quaternionhandle

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Production

	Namespace			:	zeabus_library
*/

#include	<zeabus_library/zeabus_rotation/rotation_handle.h>

#define _DEBUG_ROTATION_VALUE_

namespace zeabus_library{

namespace zeabus_rotation{

	RotationHandle::RotationHandle(){
		temporary_matrix.resize( 3 , 3 );
	}

	size_t RotationHandle::start_rotation( boost::numeric::ublas::matrix< double >& value 
							, boost::numeric::ublas::matrix< double >& result ){
		if( ! ( value.size1() == 3 && value.size2() == 1 ) ){
			zeabus_library::print_error( "zeabus_library::zeabus_rotation::RotationHandle::start_rotation value wrong size" );
			return zeabus_library::ERROR_SIZE_MATRIX;
		}
		else if( ! ( result.size1() == 3 && result.size2() == 1 ) ){
			zeabus_library::print_error( "zeabus_library::zeabus_rotation::RotationHandle::start_rotation value wrong size" );
			return zeabus_library::ERROR_SIZE_MATRIX;
		}
		this->update_rotation();
		this->matrix_rotation( this->temporary_matrix , false );

		zeabus_library::matrix::product( temporary_matrix , value , result );

		#ifdef _DEBUG_ROTATION_VALUE_
			this->get_RPY( this->diff_euler[0] , this->diff_euler[1] , this->diff_euler[2] );
			zeabus_library::vector::print("result matrix " , result );
			zeabus_library::matrix::print("Matrix rotation" , this->temporary_matrix );
			zeabus_library::vector::print("value matrix " , value );
			printf("Diff roll : pitch : yaw <====> %8.4lf : %8.4lf : %8.4lf\n"
					, this->diff_euler[0] , this->diff_euler[1] , this->diff_euler[2] );
		#endif

		return zeabus_library::NO_ERROR;
		
	}

}

}
