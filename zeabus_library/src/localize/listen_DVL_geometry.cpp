/*
	File name			:	listen_DVL_geometry.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2019 , JAN 09
	Date last modified	:	2018 , ??? ??
	Purpose				:	
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Product

	Namespace			:	zeabus_library/localize
*/

#include	<zeabus_library/localize/listen_DVL_geometry.h>

namespace zeabus_library{

namespace localize{

	ListenDVLGeometry::ListenDVLGeometry(
						boost::numeric::ublas::matrix< double >& variable ){ 
		variable.resize( 3 , 1 );
		this->velocity = variable;	
	}

	size_t ListenDVLGeometry::register_quaternion( 
							zeabus_library::rotation::Quaternion& variable ){
		if( variable.size1() != 3 && variable.size2() != 1 ){
			zeabus_library::print_error(
				"zeabus_library::~::register_quaternion wrong size of matrix to register ");
			return zeabus_library::ERROR_SIZE_MATRIX;
		}
		this->velocity = variable;
		return zeabus_library::NO_ERROR;
	}

	void ListenDVLGeometry::callback( const zeabus_library::IMUQuaternion& message ){
		this->velocity = message.twist.linear.x;
		this->velocity = message.twist.linear.y;
		this->velocity = message.twist.linear.z;
	}


}

}
