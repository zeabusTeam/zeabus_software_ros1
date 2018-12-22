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

#include	<zeabus_library/euler.h>

#include	<boost/numeric/ublas/matrix.hpp>

double degree( double degree ){
	return zeabus_library::euler::degree_domain( degree );
}

int main(){

	zeabus_library::zeabus_rotation::RotationHandle rotation_handle;

	boost::numeric::ublas::matrix< double > value;
	boost::numeric::ublas::matrix< double > result;
	value.resize( 3 , 1 );
	result.resize( 3 , 1 );

	std::cout << "Welcome to test zeabus_lirary test_rotation_handle\n";

	std::cout << "First to Third will test each rotation for gravity value is 1\n";

	std::cout << "Please interest in inertia frame of NED and Plane system\n";
	// for test system in 1 , 2 , 3 test
	rotation_handle.set_start_frame( 0 , 0 , 0 );
	value( 0 , 0 ) = 0 ;
	value( 1 , 0 ) = 0 ;
	value( 2 , 0 ) = 1 ;

	std::cout << "\nFirst Test yaw is 90 \n";
	rotation_handle.set_target_frame( 0 , 0 , degree( 90 ) );
	rotation_handle.start_rotation( value , result );

	std::cout << "\nSecond Test pitch 90 \n";
	rotation_handle.set_target_frame( 0 , degree(90) , 0);
	rotation_handle.start_rotation( value , result );

	std::cout << "\nThird Test roll 90\n";
	rotation_handle.set_target_frame( degree(90) , 0 , 0);
	rotation_handle.start_rotation( value , result );

	return 0; 

}
