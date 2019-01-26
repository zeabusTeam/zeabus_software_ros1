/*
	File name			:	localize_01.cpp
	Author				:	Supasan Komonlit	
	Date created		:	2019 , JAN 09
	Date last modified	:	2019 , JAN 26
	Purpose				:	For localize by DVL , IMU 1 , Pressure

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.2.0
	status				:	Using

	Namespace			:	None
*/

#include	<stdio.h>

#include	<ros/ros.h>

#include	<zeabus_library/Odometry.h>

#include	<boost/numeric/ublas/matrix.hpp>

#include	<geometry_msgs/TwistWithCovarianceStamped.h>

#include	<zeabus_library/IMUQuaternion.h>

#include	<zeabus_library/rotation/quaternion.h>

#include	<zeabus_library/localize/listen_IMUQuaternion.h>

#include	<zeabus_library/localize/listen_DVL.h>

#include	<zeabus_library/localize/listen_pressure_nav.h>

#define _TEST_RECEVIE_VALUE
#define _PRINT_OUTPUT_
#define _DEBUG_JUMP_VALUE_

int main( int argv , char** argc ){

	ros::init( argv , argc , "localize" );

	ros::NodeHandle ph("~"); // Handle for manage param from launch file
	ros::NodeHandle nh(""); // Handle for manage about ths file in ros system

///////////////////////////////-- PARAMETER PART --/////////////////////////////////////////////
	std::string topic_output;
	std::string topic_imu;
	std::string topic_dvl;
	std::string topic_pressure;
	int frequency;

	ph.param< std::string >("topic_output" , topic_output , "/localize/state");
	ph.param< std::string >("topic_imu" , topic_imu , "/sensor/imu/node");
	ph.param< std::string >("topic_dvl" , topic_dvl , "/sensor/dvl/node");
	ph.param< std::string >("topic_pressure" , topic_pressure , "/sensor/pressure/node");
	ph.param< int >("frequency" , frequency , 60 );

	double period = 1.0 / frequency ;

////////////////////////////////////////////////////////////////////////////////////////////////

	ros::Rate rate( frequency );

	zeabus_library::Odometry message; // for send output
	zeabus_library::Odometry previous_message;
	zeabus_library::Point3 temp_linear_imu;
	previous_message.velocity.linear.x = 0 ;
	previous_message.velocity.linear.y = 0 ;
	previous_message.velocity.linear.z = 0 ;

	zeabus_library::localize::ListenIMUQuaternion listen_imu( &(message.pose.quaternion) 
															, &(message.velocity.angular) 
															, &(temp_linear_imu) );

	zeabus_library::localize::ListenPressureNav listen_pressure( &(message.pose.position.z) );
	printf("Position of message.pose.position.z is %x \n" , &(message.pose.position.z ) );
	zeabus_library::localize::ListenDVL listen_dvl( &(message.velocity.linear) );

	#ifdef _PRINT_OUTPUT_
		zeabus_library::rotation::Quaternion quaternion;
		double euler[3];
	#endif

	double adding_x;
	double adding_y;

////////////////////////////////////-- ROS SYSTEM --/////////////////////////////////////////////
	ros::Subscriber sub_imu = nh.subscribe( topic_imu , 1 
									, &zeabus_library::localize::ListenIMUQuaternion::callback
									, &listen_imu );
	ros::Subscriber sub_dvl = nh.subscribe( topic_dvl , 1 
									, &zeabus_library::localize::ListenDVL::callback
									, &listen_dvl );
	ros::Subscriber sub_pressure = nh.subscribe( topic_pressure , 1 
									, &zeabus_library::localize::ListenPressureNav::callback
									, &listen_pressure );

	ros::Publisher tell_auv_state = 
						nh.advertise< zeabus_library::Odometry >( topic_output , 1 );

////////////////////////////////////-- PART LOCALIZE --//////////////////////////////////////////

	while( nh.ok() ){
		rate.sleep();
		ros::spinOnce();
		adding_x = message.velocity.linear.x + previous_message.velocity.linear.x * period;
		adding_x = message.velocity.linear.x + previous_message.velocity.linear.x * period;
		if( adding_x > 0.001 || adding_x < -0.001 ) message.pose.position.x += adding_x;
		if( adding_y > 0.001 || adding_y < -0.001 ) message.pose.position.y += adding_y;
		tell_auv_state.publish( message );
		previous_message.velocity.linear = message.velocity.linear;

		#ifdef _PRINT_OUTPUT_
			quaternion.set_quaternion( message.pose.quaternion );
			quaternion.get_RPY( euler[0] , euler[1] , euler[2] );
			printf("Adding  :%20.6lf%20.6lf\n" , adding_x , adding_y );
			printf("POSITION:%10.3lf%10.3lf%10.3lf%10.3lf%10.3lf%10.3lf\n"
				, message.pose.position.x , message.pose.position.y , message.pose.position.z 
				, euler[0] , euler[1] , euler[2] );
			printf("LINEAR  :%10.3lf%10.3lf%10.3lf%10.3lf%10.3lf%10.3lf\n"
				, message.velocity.linear.x , message.velocity.linear.y , message.velocity.linear.z 
				, message.velocity.angular.x , message.velocity.angular.y , message.velocity.angular.z );
		#endif
		#ifdef _DEBUG_JUMP_VALUE_
			if( adding_x > 2 || adding_x < -2 ) break;
			if( adding_y > 2 || adding_y < -2 ) break;
		#endif
	}
	printf("Message velocity linear x %10.3lf\n" , message.velocity.linear.x );
	printf("Message velocity linear y %10.3lf\n" , message.velocity.linear.y );
}
