/*
	File name			:	convert_inverse_rotation.cpp
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 05
	Date last modified	:	2018 , ??? ??
	Purpose				:	This is header of code for use metrix trasform by orientation

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	testing

	Namespace			:	zeabus_library
*/

#include	<zeabus_library/convert_rotation.h>
//#define _DEBUG_ROTATION_


namespace zeabus_library{

	void MatrixRotation::inverse_roll_rotation( double roll 
										, boost::numeric::ublas::matrix< double >& result ){
		this->check_dimension( result );	
		result(0,0) = 1		; result(0,1) = 0				; result(0,2) = 0				; 
		result(1,0) = 0		; result(1,1) = cos(roll)		; result(1,2) = -1*sin(roll)	; 
		result(2,0) = 0		; result(2,1) = -1*sin(roll)	; result(2,2) = -1*cos(roll)	; 
	}

	void MatrixRotation::inverse_pitch_rotation( double pitch 
										, boost::numeric::ublas::matrix< double >& result ){
		this->check_dimension( result );	
		result(0,0) = cos(pitch)	; result(0,1) = 0	; result(0,2) = sin(pitch)		; 
		result(1,0) = 0				; result(1,1) = 1	; result(1,2) = 0				; 
		result(2,0) = -1*sin(pitch)	; result(2,1) = 0	; result(2,2) = -1*cos(pitch)	; 
	}

	void MatrixRotation::inverse_yaw_rotation( double yaw 
										, boost::numeric::ublas::matrix< double >& result ){
		this->check_dimension( result );	
		result(0,0) = cos(yaw)		; result(0,1) = -1*sin(yaw)	; result(0,2) = 0		; 
		result(1,0) = -1*sin(yaw)	; result(1,1) = -1*cos(yaw)	; result(1,2) = 0		; 
		result(2,0) = 0				; result(2,1) = 0			; result(2,2) = 1		; 
	}

	void MatrixRotation::inverse_all_rotation( double roll , double pitch , double yaw 
										, boost::numeric::ublas::matrix< double >& result ){
		#ifdef _DEBUG_ROTATION_
			printf("Calculate All rotation\n");
		#endif
		roll *= -1;
		pitch *= -1;
		yaw *= -1; 
		this->check_dimension( result );	
		this->inverse_roll_rotation( roll , this->temp_roll_rotation );
		this->inverse_pitch_rotation( pitch , this->temp_pitch_rotation );
		this->inverse_yaw_rotation( yaw , this->temp_yaw_rotation );
		result = boost::numeric::ublas::prod( temp_roll_rotation , temp_pitch_rotation );
		result = boost::numeric::ublas::prod( result , temp_yaw_rotation );
		#ifdef _DEBUG_ROTATION_
			printf("Finish Calculate All rotation\n");
		#endif
	}

}
