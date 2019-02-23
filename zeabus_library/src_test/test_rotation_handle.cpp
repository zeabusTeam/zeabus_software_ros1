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

#include	<zeabus_library/rotation/rotation_handle.h>

#include	<zeabus_library/vector.h>

#include	<zeabus_library/matrix.h>

#include	<zeabus_library/euler.h>

#include	<boost/numeric/ublas/matrix.hpp>

double degree( double degree ){
	return zeabus_library::euler::degree_domain( degree );
}

void result_test( int number_test , boost::numeric::ublas::matrix< double >& value
								  , boost::numeric::ublas::matrix< double >& result
								  , boost::numeric::ublas::matrix< double >& expected ){
	if( abs( result( 0 , 0 ) - expected( 0 , 0 ) ) <= EPSILON &&
		abs( result( 1 , 0 ) - expected( 1 , 0 ) ) <= EPSILON &&
		abs( result( 2 , 0 ) - expected( 2 , 0 ) ) <= EPSILON ){
		printf("\nTest number %d is correct\n" , number_test );
	}
	else{
		printf("\nTest number %d is wrong\n" , number_test );
		zeabus_library::vector::print( "Value before rotation" , value );
		zeabus_library::vector::print( "Result after rotation" , result );
		zeabus_library::vector::print( "Expected value after " , expected );
	}
}

void input_value( boost::numeric::ublas::matrix< double >& vector 
					, double num1 , double num2 , double num3 ){
	vector( 0 , 0 ) = num1;
	vector( 1 , 0 ) = num2;
	vector( 2 , 0 ) = num3;
}

int main(){

	zeabus_library::rotation::RotationHandle rotation_handle;

	boost::numeric::ublas::matrix< double > value;
	boost::numeric::ublas::matrix< double > result;
	boost::numeric::ublas::matrix< double > expected;
	value.resize( 3 , 1 );
	result.resize( 3 , 1 );
	expected.resize( 3 , 1 );

	std::cout << "Welcome to test zeabus_lirary test_rotation_handle\n";

	std::cout << "First to Third will test each rotation for gravity value is 1\n";

	std::cout << "Please interest in inertia frame of NED and Plane system\n";
	// for test system in 1 , 2 , 3 test
	rotation_handle.set_start_frame( 0 , 0 , 0 );
	value( 0 , 0 ) = 0 ;
	value( 1 , 0 ) = 0 ;
	value( 2 , 0 ) = 1 ;

	rotation_handle.set_target_frame( 0 , 0 , 0 );
	rotation_handle.start_rotation( value , result );
	input_value( expected , 0 , 0 , 1 );
	result_test( 0 , value , result , expected );

	rotation_handle.set_target_frame( 0 , 0 , degree( 90 ) );
	rotation_handle.start_rotation( value , result );
	input_value( expected , 0 , 0 , 1 );
	result_test( 1 , value , result , expected );

	rotation_handle.set_target_frame( 0 , degree(90) , 0);
	rotation_handle.start_rotation( value , result );
	input_value( expected , -1 , 0 , 0 );
	result_test( 2 , value , result , expected );

	rotation_handle.set_target_frame( degree(90) , 0 , 0);
	rotation_handle.start_rotation( value , result );
	input_value( expected , 0 , 1 , 0 );
	result_test( 3 , value , result , expected );

	rotation_handle.set_target_frame( 0 , degree(45) , degree(45) );
	rotation_handle.start_rotation( value , result );
	input_value( expected , -0.707 , 0 , 0.707 );
	result_test( 4 , value , result , expected );

	rotation_handle.set_target_frame( degree(45) , degree(45) , degree(45) );
	rotation_handle.start_rotation( value , result );
	input_value( expected , -0.707 , 0.5 , 0.5 );
	result_test( 5 , value , result , expected );

	rotation_handle.set_target_frame( degree(0) , degree(180) , degree(0) );
	rotation_handle.start_rotation( value , result );
	input_value( expected , 0 , 0 , -1 );
	result_test( 6 , value , result , expected );

	return 0; 

}
