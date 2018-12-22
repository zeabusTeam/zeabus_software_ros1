/*
	File name			:	test_rotation_handle.cpp
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 22
	Date last modified	:	2018 , ??? ??
	Purpose				:	For test_rotation_handle
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Production

	Namespace			:	None < main program>
*/

#include	<stdio.h>

#include	<iostream>

#include	<zeabus_library/zeabus_rotation/rotation_handle.h>

#include	<boost/numeric/ublas/matrix.hpp>

int main(){

	zeabus_library::zeabus_rotation::RotationHandle rotation_handle;

	boost::numeric::ublas::matrix< double > value;
	boost::numeric::ublas::matrix< double > result;
	value.resize( 3 , 1 );
	result.resize( 3 , 1 );

	std::cout << "Welcome to test zeabus_lirary test_rotation_handle\n";

	std::cout << "First to Third will test each rotation for gravity value is 1\n";

	std::cout << "Please interest in inertia frame of NED and Plane system\n";

	std::cout << "\nFirst Test\n";
	rotation_handle.set_start_frame( 0 , 0 , 0 );
	rotation_handle.set_target_frame( 0 , 0 , 90 );
	value( 0 , 0 ) = 0 ;
	value( 1 , 0 ) = 0 ;
	value( 2 , 0 ) = 1 ;
	rotation_handle.start_rotation( value , result );

	return 0; 

}
