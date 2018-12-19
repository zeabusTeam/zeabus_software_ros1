/*
	File name			:	base_quaternion.cpp
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 18
	Date last modified	:	2018 , ??? ??
	Purpose				:	For count_time
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Production

	Namespace			:	None < main program>
*/

#include	<stdio.h>

#include	<iostream>

#include	<zeabus_library/matrix.h>

#include	<zeabus_library/quaternion_handle.h>

//#define _test_quaternion_to_euler_

#define _test_euler_to_quaternion_

int main(){
	zeabus_library::QuaternionHandle quaternion;
	zeabus_library::MatrixRotation matrix_handle;

	double w , x , y , z ;
	double roll , pitch , yaw ;
	#ifdef _test_quaternion_to_euler_ 


	std::cout	<< "Welcome to test base of quaternion for zeabus_library please input your "
				<< "qauternion \n"
				<< "w : ";
	std::cin	>> w;
	std::cout	<< "x : ";
	std::cin	>> x;
	std::cout	<< "y : ";
	std::cin	>> y;
	std::cout	<< "z : ";
	std::cin	>> z;

	quaternion.set_quaternion( w , x , y , z );
	quaternion.get_RPY( roll , pitch , yaw );

	printf( "roll : pitch : yaw <===========> %8.6lf : %8.6lf : %8.6lf \n" , roll , pitch ,yaw );

	#endif	

	#ifdef _test_euler_to_quaternion_

	std::cout	<< "Welcome to test base of quaternion for zeabus_library please input your "
				<< "euler \n"
				<< "roll : ";
	std::cin	>> roll;
	std::cout	<< "pitch : ";
	std::cin	>> pitch;
	std::cout	<< "yaw : ";
	std::cin	>> yaw;

	quaternion.set_quaternion( roll , pitch , yaw );
	zeabus_library::matrix::print("Result quaternion" , quaternion.matrix );

	#endif
}
