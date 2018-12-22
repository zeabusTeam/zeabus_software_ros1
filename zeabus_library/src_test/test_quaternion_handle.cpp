/*
	File name			:	test_quaternion_handle.cpp
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 22
	Date last modified	:	2018 , ??? ??
	Purpose				:	For test_quaternion_handle
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Production

	Namespace			:	None < main program>
*/

#include	<stdio.h>

#include	<iostream>

#define _ROTATION_EULER_
#define _DEBUG_ROTATION_EULER_
#define _ROTATION_QUATERNION_
#define _DEBUG_ROTATION_QUATERNION_

#include	<zeabus_library/zeabus_rotation/quaternion_handle.h>

void input_value( std::string message , double& roll , double& pitch , double& yaw ){
	std::cout << message << "\n";
	std::cout << "roll : "		;	std::cin >> roll; 
	std::cout << "pitch : "		;	std::cin >> pitch; 
	std::cout << "yaw : "		;	std::cin >> yaw; 
}

int main(){

	zeabus_library::zeabus_rotation::QuaternionHandle quaternion_handle;
	double roll , pitch , yaw;

	std::cout << "Standard test rotation";
	
	quaternion_handle.set_start_frame( 0 , 0 , 0 );	
	quaternion_handle.set_target_frame( 0 , 0 , 90 );
	
	quaternion_handle.update_rotation();

	std::cout << "FINISH TEST QUATERNION HANDLE\n";

}
