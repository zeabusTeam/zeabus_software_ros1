/*
	File name			:	matrix_rotation.cpp
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 05
	Date last modified	:	2018 , DEC 27
	Purpose				:	This is header of code for use metrix trasform by orientation

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.1.1
	status				:	Stop because failure when apply with acceleration data from IMU

	Namespace			:	zeabus_library
*/

#include	<zeabus_library/rotation/matrix_rotation.h>
//#define _DEBUG_ROTATION_


namespace zeabus_library{

namespace rotation{

	MatrixRotation::MatrixRotation(){
		temp_roll_rotation.resize( 3 , 3 );
		temp_pitch_rotation.resize( 3 , 3 );
		temp_yaw_rotation.resize( 3 , 3 );
		temp_result.resize( 3 , 3 );
		#ifdef _DEBUG_ROTATION_
			printf("Finish set up call MatrixRotation\n");
		#endif
	}

	void MatrixRotation::roll_rotation( double roll 
										, boost::numeric::ublas::matrix< double >& result ){
		this->check_dimension( result );	
		result(0,0) = 1		; result(0,1) = 0				; result(0,2) = 0			; 
		result(1,0) = 0		; result(1,1) = cos(roll)		; result(1,2) = -1*sin(roll); 
		result(2,0) = 0		; result(2,1) = sin(roll)		; result(2,2) = cos(roll)	; 
	}

	void MatrixRotation::pitch_rotation( double pitch 
										, boost::numeric::ublas::matrix< double >& result ){
		this->check_dimension( result );	
		result(0,0) = cos(pitch)	; result(0,1) = 0	; result(0,2) = sin(pitch)		; 
		result(1,0) = 0				; result(1,1) = 1	; result(1,2) = 0				; 
		result(2,0) = -1*sin(pitch)	; result(2,1) = 0	; result(2,2) = cos(pitch)		; 
	}

	void MatrixRotation::yaw_rotation( double yaw 
										, boost::numeric::ublas::matrix< double >& result ){
		this->check_dimension( result );	
		result(0,0) = cos(yaw)		; result(0,1) = -1*sin(yaw)	; result(0,2) = 0		; 
		result(1,0) = sin(yaw)		; result(1,1) = cos(yaw)	; result(1,2) = 0		; 
		result(2,0) = 0				; result(2,1) = 0			; result(2,2) = 1		; 
	}

	void MatrixRotation::all_rotation( double roll , double pitch , double yaw 
										, boost::numeric::ublas::matrix< double >& result ){
		#ifdef _DEBUG_ROTATION_
			printf("Calculate All rotation\n");
		#endif
		this->check_dimension( result );	
		this->roll_rotation( roll , this->temp_roll_rotation );
		this->pitch_rotation( pitch , this->temp_pitch_rotation );
		this->yaw_rotation( yaw , this->temp_yaw_rotation );
		this->temp_result = boost::numeric::ublas::prod(	temp_roll_rotation
															, temp_pitch_rotation );
		result = boost::numeric::ublas::prod( temp_result , temp_yaw_rotation );
		#ifdef _DEBUG_ROTATION_
			printf("Finish Calculate All rotation\n");
		#endif
	}

	void MatrixRotation::check_dimension( boost::numeric::ublas::matrix< double >& data ){
		if( ( data.size1() ) == 3 && ( data.size2() == 3 ));
		else{
			throw std::overflow_error("Wrong size of Matrix\n");
		}
	}

	void MatrixRotation::print_all_temp(){
		zeabus_library::matrix::print("temp roll matrix" , this->temp_roll_rotation );
		zeabus_library::matrix::print("temp pitch matrix" , this->temp_pitch_rotation );
		zeabus_library::matrix::print("temp yaw matrix" , this->temp_yaw_rotation );
	}

}

}
