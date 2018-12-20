/*
	File name			:	test_color_text.cpp
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 20
	Date last modified	:	2018 , ??? ??
	Purpose				:	For test color text can use or not
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Production

	Namespace			:	None < main program>
*/

#include	<stdio.h>

#include	<zeabus_library/vector.h>

int main(){

	boost::numeric::ublas::matrix< double > test_01(1 , 1 );
	boost::numeric::ublas::matrix< double > test_02(4 , 1 );
	boost::numeric::ublas::matrix< double > test_03(4 , 1 );

	size_t error_code;

	error_code = zeabus_library::vector::multiplication( test_01 , test_02 , test_03 );

	return 0;

}
