/*
	File name			:	convert_rotation.cpp
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 05
	Date last modified	:	2018 , ??? ??
	Purpose				:	This is header of code for use metrix trasform by orientation

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.0.1
	status				:	product

	Namespace			:	zeabus_library
*/

#include	<zeabus_library/convert_rotation.h>

namespace zeabus_library{

	MatrixRotation::MatrixRotation(){
		temp_roll_rotation.resize( 3 , 3 );
		temp_pitch_rotation.resize( 3 , 3 );
		temp_yaw_rotation.resize( 3 , 3 );
	}

	void MatrixRotation::roll_rotation( double roll 
										, boost::numeric::ublas::matrix< double >& result ){
		
	}

	void MatrixRotation::pitch_rotation( double pitch 
										, boost::numeric::ublas::matrix< double >& result ){

	}

	void MatrixRotation::yaw_rotation( double yaw 
										, boost::numeric::ublas::matrix< double >& result ){

	}

	void MatrixRotation::all_rotation( double roll , double pitch , double yaw 
										, boost::numeric::ublas::matrix< double >& result ){

	}

	void check_dimension( boost::numeric::ublas::matrix< double >& result ){
		if( ( result.size1() ) == 3 && ( result.size2() == 3 ));
		else{
			throw std::overflow_error("Wrong size of Matrix\n");
		}
	}

}
