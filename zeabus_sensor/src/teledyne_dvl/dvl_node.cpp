/*
	File name			:	dvl_node.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2019 , JAN 12
	Date last modified	:	2019 , ??? ??
	Purpose				:	This is file to use read connect and convert dvl data in ros system

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Maintain

	Namespace			:	None
*/

#include	<stdio.h>

#include	<iostream>

#include	<zeabus_library/rotation/rotation_handle.h>

#include	<zeabus_library/convert/Point3.h>

#include	<zeabus_library/text_color.h>

#include	<zeabus_library/localize/listen_IMUQuaternion.h>

#include	<zeabus_library/localize/listen_DVL.h>

#define _DEBUG_JUMP_VALUE_

int main( int argc , char** argv ){

	ros::init( argc , argv , "node_dvl" );

	ros::NodeHandle nh(""); // Handle for manage about this file in ros system
	ros::NodeHandle ph("~"); // Handle for manage param from launch

/////////////////////////////////-- PARAMETER PART --///////////////////////////////////////////

	std::string topic_input;
	std::string topic_output;
	std::string topic_imu;
	int frequency;

	ph.param< std::string >("topic_input_node_dvl" , topic_input , "/sensor/dvl/port");
	ph.param< std::string >("topic_imu" , topic_imu , "/sensor/imu/node");
	ph.param< std::string >("topic_output_node_dvl" , topic_output , "/sensor/dvl/node");
	ph.param< int >("frequency" , frequency , 100 );

///////////////////////////////////////////////////////////////////////////////////////////////

	ros::Rate rate( frequency );

	zeabus_library::rotation::RotationHandle rh;

	boost::numeric::ublas::matrix< double > receive_matrix;
	boost::numeric::ublas::matrix< double > result_matrix;
	zeabus_library::Point3 data_dvl;

	zeabus_library::Point4 data_quaternion;
	zeabus_library::Point3 data_angular_velocity;
	zeabus_library::Point3 data_linear_acceleration;

	zeabus_library::localize::ListenDVL listen_dvl( &data_dvl );
	zeabus_library::localize::ListenIMUQuaternion listen_imu( &data_quaternion 
												, &data_angular_velocity
												,  &data_linear_acceleration );
	listen_dvl.set_limit_count( 10 );
	listen_imu.set_limit_count( 10 );

	receive_matrix.resize( 3 , 1 );
	result_matrix.resize( 3 , 1 );
	rh.set_start_frame( PI , 0 , 0 );
	rh.set_target_frame( 0 , 0 , 0 );
	
/////////////////////////////////////-- ROS SYSTEM --///////////////////////////////////////////

	ros::Subscriber sub_quaternion = nh.subscribe( topic_imu , 1 
						, &zeabus_library::localize::ListenIMUQuaternion::callback_quaternion
						, &listen_imu );

	ros::Subscriber sub_dvl = nh.subscribe( topic_input , 1 
						, &zeabus_library::localize::ListenDVL::callback
						, &listen_dvl );

	ros::Publisher tell_dvl = nh.advertise< zeabus_library::Point3 >( topic_output , 1 );

////////////////////////////////////////////////////////////////////////////////////////////////

	while( nh.ok() ){
		rate.sleep();
		ros::spinOnce();
		listen_dvl.count --;
		listen_imu.count --;
		if( listen_dvl.count < 0 ) zeabus_library::bold_red( "Fatal! DVL missing receive\n\n");
		if( listen_imu.count < 0 ) zeabus_library::bold_red( "Fatal! IMU missing receive\n\n");
		rh.set_target_frame( data_quaternion );
		zeabus_library::convert::Point3_to_matrix( data_dvl , receive_matrix );
		#ifdef _DEBUG_JUMP_VALUE_
			printf("Receive value %8.3lf\t%8.3lf\n" , data_dvl.x , data_dvl.y );
		#endif
		rh.target_rotation( receive_matrix , result_matrix );
		zeabus_library::convert::matrix_to_Point3( result_matrix , data_dvl );
		tell_dvl.publish( data_dvl );
		#ifdef _DEBUG_JUMP_VALUE_
			printf("send value %8.3lf\t%8.3lf\n\n\n" , data_dvl.x , data_dvl.y );
		#endif

		#ifdef _DEBUG_JUMP_VALUE_
			if( data_dvl.x > 2 || data_dvl.x < -2 ) break;
			if( data_dvl.y > 2 || data_dvl.y < -2 ) break;
		#endif
	}
}
