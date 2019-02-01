/*
	File name			:	tf_port_imu.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2018 , FEB 31
	Date last modified	:	2018 , FEB 31
	Purpose				:	This is read connect data with IMU and TF ROS

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.1.0
	status				:	Maintainance

	Namespace			:	None
*/

#include	<ros/ros.h>

#include	<sensor_msgs/Imu.h>

#include	<zeabus_library/sensor/lord_microstrain.h>
#include	<zeabus_library/convert/bytes.h>

#include	<tf/transform_broadcaster.h>

#include	<zeabus_static/convert/TF_data.h>

#include	<geometry_msgs/QuaternionStamped.h>

//#define	_DEBUG_SPILT_DATA_ // For debug about get value each part of packet

//#define _PUBLISH_DATA_ 

#ifndef PI
	#define PI 3.14159265
#endif

typedef double tfScalar;

namespace Asio = boost::asio;
namespace DataIMU = zeabus_library::sensor::MIP_COMMUNICATION::DATA::IMU_DATA_SET ;
namespace SC = zeabus_static::convert ;

int main( int argv , char** argc ){
	
	ros::init( argv , argc , "port_imu");

	ros::NodeHandle ph("~"); // Handle for manage param from launch
	ros::NodeHandle nh(""); // Handle for manage about this file in ros system

////////////////////////////////////-- PARAMETER PART --/////////////////////////////////////////
	std::string topic_quaternion; // send only quaterion frame imu
	std::string port_name; 
	std::string frame_id_velocity; // for collect angular velocity
	std::string frame_id_acceleration; // for collect linear acceleration
	std::string frame_id_quaternion; 

	std::string topic_sensor; // for case you want to publish data
	
	double euler_ZYX[3] = { 0 , 0 , 0 };
	int frequency;

	ph.param< std::string >( "frame_id_acceleration" 
								, frame_id_acceleration , "imu_acceleration" );

	ph.param< std::string >( "frame_id_velocity" 
								, frame_id_velocity , "imu_velocity" );

	ph.param< std::string >( "frame_id_quaternion" 
								, frame_id_quaternion , "imu_quaternion" );

	ph.param< std::string >( "name_port_imu" , port_name 
								, "/dev/microstrain/3dm_gx5_45_0000__6251.65901");

	ph.param< std::string >( "topic_quaternion" , topic_quaternion , "/sensor/imu/quaternion");

	ph.param< std::string >( "topic_publish" , topic_sensor , "/sensor/imu/port");

	
	ph.param< double >( "offset_roll" , euler_ZYX[0] , 0.0 );
	ph.param< double >( "offset_pitch" , euler_ZYX[1] , 0.0 );
	ph.param< double >( "offset_yaw" , euler_ZYX[2] , PI/2 ) ;

	ph.param< int >("frequency_imu" , frequency , 100 );

///////////////////////////////////////-- TF PART --/////////////////////////////////////////////

	static tf::TransformBroadcaster broadcast_velocity; // angular_velocity
	static tf::TransformBroadcaster broadcast_acceleration; // acceleration_sender

	tf::Quaternion tf_quaternion;
	tf_quaternion.setEulerZYX( euler_ZYX[2] , euler_ZYX[1] , euler_ZYX[0] );

	tf::Transform transform_acceleration;
	transform_acceleration.setRotation( tf_quaternion );
	tf::Transform transform_velocity;
	transform_velocity.setRotation( tf_quaternion );

////////////////////////////////////-- ROS SYSTEM PART --////////////////////////////////////////

	ros::Time time;

	ros::Rate rate( frequency );
	geometry_msgs::QuaternionStamped QS;

	sensor_msgs::Imu sensor; //  for collecting message come form sensor

	QS.header.frame_id = frame_id_quaternion;

	ros::Publisher pub_quaternion = ph.advertise< geometry_msgs::QuaternionStamped >( 
		topic_quaternion , 1 );

	ros::Publisher pub_sensor = ph.advertise< sensor_msgs::Imu >( 
		topic_quaternion , 1 );


/////////////////////////////////-- CONNECT IMU PART --/////////////////////////////////////////

	bool result ;

	zeabus_library::sensor::LordMicrostrain imu( port_name );

	imu.open_port( result );

	imu.set_option_port( Asio::serial_port_base::flow_control( 
							Asio::serial_port_base::flow_control::none ) );
	imu.set_option_port( Asio::serial_port_base::parity( 
							Asio::serial_port_base::parity::none ) );
	imu.set_option_port( Asio::serial_port_base::stop_bits( 
							Asio::serial_port_base::stop_bits::one ) );
	imu.set_option_port( Asio::serial_port_base::character_size( (size_t) 8 ) );
	
	imu.is_open( result );
	if( ! result ) return 1;

	result = false;
	int IMU_base_rate = 0;

	do{
		printf( "IMU set to ide ");
		imu.command_idle( result );
		rate.sleep();
	}while( ( ! result ) && nh.ok() );

	do{
		printf( "IMU get base rate ");
		imu.sensor_get_IMU_base_rate( result , IMU_base_rate );
		rate.sleep();
	}while( ( ! result ) && nh.ok() );

	imu.set_IMU_rate( IMU_base_rate / frequency );

	do{
		printf( "Set type message ");
		imu.sensor_init_setup_IMU_format( 3 );
		imu.sensor_add_message_type( DataIMU::SCALED_ACCELEROMETER_VECTOR );
		imu.sensor_add_message_type( DataIMU::SCALED_GYRO_VECTOR );
		imu.sensor_add_message_type( DataIMU::CF_QUATERNION );
		imu.sensor_setup_IMU_format( result );
	}while( ( ! result ) && ph.ok() );

	do{
		printf( "Set setting format ");
		imu.sensor_save_message_format( result );
	}while( ( ! result ) && nh.ok() );

	do{
		printf( "Set enable stream ");
		imu.sensor_enable_data_stream( true , false , result );
	}while( ( ! result ) && nh.ok() );

	do{
		printf( "Set resume for data stream ");
		imu.command_resume( result );	
	}while( ( ! result ) && nh.ok() );

	std::vector< uint8_t > data_stream;
	while( ph.ok() ){
		rate.sleep();
		imu.read_data_stream( data_stream , result );
		if( result ){
			for( int run = 4 ; run < data_stream.size() ; ){
				if( data_stream[ run ] == DataIMU::SCALED_ACCELEROMETER_VECTOR ){
					#ifdef _DEBUG_SPILT_DATA_
						printf("Position %d Accelerometer--> %2X\n" , run, data_stream[run] );
					#endif
					run += 1;
					zeabus_library::convert::uint8_t_to_Vector3( sensor.linear_acceleration
															, data_stream , run );
					run += 12 ;
				}
				else if( data_stream[run] == DataIMU::SCALED_GYRO_VECTOR ){
					#ifdef _DEBUG_SPILT_DATA_
						printf("Position %d SCALED GYRO --> %2X\n" , run ,data_stream[run]);
					#endif
					run += 1;

					zeabus_library::convert::uint8_t_to_Vector3( sensor.angular_velocity
															, data_stream , run );
					run += 12 ;
				}
				else if( data_stream[ run ] == DataIMU::CF_QUATERNION ){
					#ifdef _DEBUG_SPILT_DATA_
						printf("Position %d QUATERNION --> %2X\n" ,run ,data_stream[run]);
					#endif
					run += 1;
					zeabus_library::convert::uint8_t_to_Quaternion( sensor.orientation
															, data_stream , run );
					run += 16;
				}
				else{
					#ifdef _DEBUG_SPILT_DATA_
						printf("Skip\n");
					#endif
					run += 1;
				}
			}
			transform_acceleration.setOrigin( tf::Vector3( sensor.linear_acceleration.x
												, sensor.linear_acceleration.y
												, sensor.linear_acceleration.z ) );
			transform_velocity.setOrigin( tf::Vector3(sensor.angular_velocity.x
												, sensor.angular_velocity.y
												, sensor.angular_velocity.z ) );
			time = ros::Time::now();
			#ifdef _PUBLISH_DATA_
				sensor.header.stampe = time;
				pub_sensor.publish( sensor );
			#endif
			QS.header.stamp = time;
			QS.quaternion = sensor.orientation;
			pub_quaternion.publish( QS );
			broadcast_acceleration.sendTransform( tf::StampedTransform( transform_acceleration
				, time , "robot" , frame_id_acceleration ) );
			broadcast_velocity.sendTransform( tf::StampedTransform( transform_velocity 
				, time , "robot" , frame_id_velocity ) );	
			
		}

	}	

	int count = 0;
	do{
		printf( "IMU set to ide ");
		imu.command_idle( result );
		rate.sleep();
		count++;
	}while( ( ! result ) && count < 10 );
}
