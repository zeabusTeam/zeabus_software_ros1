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

#include	<zeabus_library/rotation/quaternion_handle.h>

double roll , pitch , yaw;

void input_value( std::string message , double& roll , double& pitch , double& yaw ){
	std::cout << message << "\n";
	std::cout << "roll : "		;	std::cin >> roll; 
	std::cout << "pitch : "		;	std::cin >> pitch; 
	std::cout << "yaw : "		;	std::cin >> yaw; 
}

void result_test( int& correct , int& wrong 
					, zeabus_library::zeabus_rotation::QuaternionHandle& quaternion_handle ){
	quaternion_handle.get_RPY( roll , pitch , yaw );
	printf("                      :    roll\t\t   pitch\t  yaw\n");
	printf("Value from quaternion : %8.4lf\t%8.4lf\t%8.4lf\n" , roll , pitch , yaw );
	printf("Value form euler      : %8.4lf\t%8.4lf\t%8.4lf\n" , quaternion_handle.diff_euler[0]
															, quaternion_handle.diff_euler[1]
															, quaternion_handle.diff_euler[2] );
	if( abs(roll - quaternion_handle.diff_euler[0] ) < EPSILON &&
		abs(pitch - quaternion_handle.diff_euler[1]) < EPSILON &&
		abs(yaw - quaternion_handle.diff_euler[2] ) < EPSILON ) correct++;
	else wrong++;
}

int main(){

	zeabus_library::rotation::QuaternionHandle quaternion_handle;
	int correct = 0 ;
	int wrong = 0 ;

	std::cout << "Standard test rotation\n";
	
	quaternion_handle.set_start_frame( 0 , 0 , 0 );	
	quaternion_handle.set_target_frame( 0 , 0 , 90 );
	quaternion_handle.update_rotation();
	result_test( correct , wrong , quaternion_handle );

	std::cout << "Second test\n";
	quaternion_handle.set_start_frame( 0 , 0 , 0 );	
	quaternion_handle.set_target_frame( 0 , -45 , 0 );
	quaternion_handle.update_rotation();
	result_test( correct , wrong , quaternion_handle );

	std::cout << "Third test\n";
	quaternion_handle.set_start_frame( 0 , 0 , 0 );	
	quaternion_handle.set_target_frame( 30 , 0 , 0 );
	quaternion_handle.update_rotation();
	result_test( correct , wrong , quaternion_handle );

	std::cout << "fourth test\n";
	quaternion_handle.set_start_frame( 0 , 0 , 0 );	
	quaternion_handle.set_target_frame( -30 , 45 , 90 );
	quaternion_handle.update_rotation();
	result_test( correct , wrong , quaternion_handle );

	std::cout << "fifth test\n";
	quaternion_handle.set_start_frame( 180 , 0 , 0 );	
	quaternion_handle.set_target_frame( 180 , 0 , 90 );
	quaternion_handle.update_rotation();
	result_test( correct , wrong , quaternion_handle );

	std::cout << "FINISH TEST QUATERNION HANDLE\n";
	std::cout << "ALL TEST " << correct + wrong << "  correct : wrong are " 
			  << correct << " : " << wrong << "\n"; 

}
