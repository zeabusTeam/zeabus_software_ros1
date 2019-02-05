#include	<iostream>

#include	<stdio.h>

#include	<tf/transform_broadcaster.h>

void print_euler( double* first , double* second , double* third ){
	printf("FIRST QUATERNION  --> %10.3lf%10.3lf%10.3lf\n" , first[0] , first[1] , first[2]);
	printf("SECOND QUATERNION --> %10.3lf%10.3lf%10.3lf\n" , second[0] , second[1] , second[2]);
	printf("THIRD QUATERNION  --> %10.3lf%10.3lf%10.3lf\n" , third[0] , third[1] , third[2]);
}

void input_value( double* array , double roll , double pitch , double yaw ){
	array[0] = roll;
	array[1] = pitch;
	array[2] = yaw;
}

#ifndef PI
	#define PI 3.14159265
#endif

int main(){

	double euler01[3];
	double euler02[3];
	double euler03[3];
	tf::Quaternion quaternion01;
	tf::Quaternion quaternion02;
	tf::Quaternion quaternion03;

	printf("Welcome to test of Quaternion operation and solution by tf::Quaternion library\n");

	printf("Please use quaternion that make form order ZYX (yaw-pitch-roll)\n");
	
	printf("\n==================== QUATERNION ROTATION q3 = q2*q1==========================\n");
	input_value( euler01 , 0 , 0 , PI/2 );
	input_value( euler02 , 0 , 0 , PI );
	quaternion01.setEulerZYX( euler01[2] , euler01[1] , euler01[0] );
	quaternion02.setEulerZYX( euler02[2] , euler02[1] , euler02[0] );
	quaternion03 = quaternion02 * quaternion01;
	tf::Matrix3x3(quaternion03).getRPY( euler03[0] , euler03[1] , euler03[2]);	
	print_euler( euler01 , euler02 , euler03 );

	printf("\n\n set 2\n");
	input_value( euler01 , 0 , 0 , 1 );
	input_value( euler02 , 0 , 0 , 1.5 );
	quaternion01.setEulerZYX( euler01[2] , euler01[1] , euler01[0] );
	quaternion02.setEulerZYX( euler02[2] , euler02[1] , euler02[0] );
	quaternion03 = quaternion02 * quaternion01;
	tf::Matrix3x3(quaternion03).getRPY( euler03[0] , euler03[1] , euler03[2]);	
	print_euler( euler01 , euler02 , euler03 );

	printf("\n\n set 3\n");
	input_value( euler01 , 0 , 0 , 1 );
	input_value( euler02 , PI , 0 , 0 );
	quaternion01.setEulerZYX( euler01[2] , euler01[1] , euler01[0] );
	quaternion02.setEulerZYX( euler02[2] , euler02[1] , euler02[0] );
	quaternion03 = quaternion02 * quaternion01;
	tf::Matrix3x3(quaternion03).getRPY( euler03[0] , euler03[1] , euler03[2]);	
	print_euler( euler01 , euler02 , euler03 );

	printf("\n\n set 4\n");
	input_value( euler01 , 3.14 , 0 , 1 );
	input_value( euler02 , 0 , 0 , 1.5 );
	quaternion01.setEulerZYX( euler01[2] , euler01[1] , euler01[0] );
	quaternion02.setEulerZYX( euler02[2] , euler02[1] , euler02[0] );
	quaternion03 = quaternion02 * quaternion01;
	tf::Matrix3x3(quaternion03).getRPY( euler03[0] , euler03[1] , euler03[2]);	
	print_euler( euler01 , euler02 , euler03 );

	printf("\n============= QUATERNION DIFFERENTIAL q3*q1 = q2 know q2 and q1 =============\n");
	input_value( euler01 , 0 , 0 , 0 );
	input_value( euler02 , 0.2 , 0.1 , 2.3 );
	quaternion01.setEulerZYX( euler01[2] , euler01[1] , euler01[0] );
	quaternion02.setEulerZYX( euler02[2] , euler02[1] , euler02[0] );
	quaternion03 = quaternion02 * quaternion01.inverse();
	tf::Matrix3x3(quaternion03).getRPY( euler03[0] , euler03[1] , euler03[2]);	
	print_euler( euler01 , euler02 , euler03 );

	

}
