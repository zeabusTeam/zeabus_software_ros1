/*
	File name			:	imu_node.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2018 , FEB 04
	Date last modified	:	2018 , ??? ??
	Purpose				:	This File will use to connect between IMU and ROS system 
							Include Convert NED -> ENU system
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.1.0
	status				:	Maintainance

	Namespace			:	None
*/
//=================================>

#include	<zeabus_library/sensor/lord_microstrain.h> // include for receive connect imu hardware
#include	<zeabus_library/convert/bytes.h> // for convert byte to float number

#include	<ros/ros.h>

#include	<sensor_msgs/Imu.h>

#include	<tf/transform_broadcaster.h>

#include	<zeabus_static/convert/TF_data.h>

#include	<geometry_msgs/QuaternionStamped.h>

#ifndef PI
	#define PI 3.14159265
#endif

#define		_ECHO_RAW_VALUE_

typedef double tfScalar;

namespace Asio = boost::asio;
namespace DataIMU = zeabus_library::sensor::MIP_COMMUNICATION::DATA::IMU_DATA_SET ;
namespace SC = zeabus_static::convert ;

int main( int argv , char** argc ){
	
	ros::init( argv , argc , "port_imu");

	ros::NodeHandle ph("~"); // Handle for manage param from launch
	ros::NodeHandle nh(""); // Handle for manage about this file in ros system

//=================================> PARAMETER PART
	std::string topic_raw_value;		// use topic publish raw value from imu for debug
	std::string topic_rotation_value;	// use topic publish value after rotation 

	std::string frame_id;				// this will add type _01 for linear acceleration
										//                    _02 for angular velocity
										// for use in tf system

	ph.param< std::string >( "imu_")

	std::string port_name;				// Hardware part
	int frequency;						// frequency will affect to set up hardware
	ph.param< std::string >( "name_port_imu" , port_name 
								, "/dev/microstrain/3dm_gx5_45_0000__6251.65901");
	ph.param< int >("frequency_imu" , frequency , 50 );


//=================================> ROS OPERATING PART

//=================================> TF PART

//=================================> CONNECT TO IMU PART

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

//=================================> START LOOP

	while( ph.ok() ){
		rate.sleep();
		imu.read_data_stream( data_stream , result );
		if( result ){
			for( int run = 4 ; run < data_stream.size() ; ){
				if( data_stream[ run ] == DataIMU::SCALED_ACCELEROMETER_VECTOR ){
					run += 1;
					zeabus_library::convert::uint8_t_to_Vector3( sensor.linear_acceleration
															, data_stream , run );
					run += 12 ;
				}
				else if( data_stream[run] == DataIMU::SCALED_GYRO_VECTOR ){
					run += 1;
					zeabus_library::convert::uint8_t_to_Vector3( sensor.angular_velocity
															, data_stream , run );
					run += 12 ;
				}
				else if( data_stream[ run ] == DataIMU::CF_QUATERNION ){
					run += 1;
					zeabus_library::convert::uint8_t_to_Quaternion( sensor.orientation
															, data_stream , run );
					run += 16;
				}
				else{
					run += 1;
				}
			}
		//=================================> CONNECTING PART

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
