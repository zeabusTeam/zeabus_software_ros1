/*
	File name			:	imu_port.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2018 , NOV 27
	Date last modified	:	2018 , ??? ??
	Purpose				:	This is file to use read connect ros and IMU

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.0.1
	status				:	Production

	Namespace			:	None
*/

#include	<ros/ros.h>

#include	<zeabus_library/IMUData.h>

#include	<zeabus_library/zeabus_sensor/lord_microstrain.h>
#include	<zeabus_library/convert_bytes.h>
#include	"./../../../zeabus_library/src/convert_bytes.cpp"

#define	_DEBUG_SPILT_DATA_

namespace Asio = boost::asio;
namespace DataIMU = zeabus_sensor::MIP_COMMUNICATION::DATA::IMU_DATA_SET ;

int main( int argv , char** argc ){
	
	ros::init( argv , argc , "main_control");

	ros::NodeHandle nh(""); // Handle for manage about this file in ros system
	ros::NodeHandle ph(""); // Handle for manage param from launch

////////////////////////////////////-- PARAMETER PART --/////////////////////////////////////////
	std::string topic_output;
	std::string port_name;
	int frequency;

	ph.param< std::string >("name_port_imu" , port_name 
								, "/dev/microstrain/3dm_gx5_45_0000__6521.65901");

	ph.param< std::string >("topic_output_port_imu" , topic_output , "/sensor/imu/port");

	ph.param< int >("frequency_imu" , frequency , 250 );

////////////////////////////////////////////////////////////////////////////////////////////////


	ros::Rate rate( frequency );

	ros::Publisher tell_data = 
			ph.advertise< zeabus_library::IMUData >( topic_output , 1 );

	zeabus_library::IMUData message;

	bool result ;

	zeabus_sensor::LordMicrostrain imu( port_name );

	imu.open_port( result );

	imu.set_option_port( Asio::serial_port_base::flow_control( 
							Asio::serial_port_base::flow_control::none ) );
	imu.set_option_port( Asio::serial_port_base::parity( 
							Asio::serial_port_base::parity::none ) );
	imu.set_option_port( Asio::serial_port_base::stop_bits( 
							Asio::serial_port_base::stop_bits::one ) );
	imu.set_option_port( Asio::serial_port_base::character_size( (size_t) 8 ) );

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
		imu.sensor_add_message_type( DataIMU::CF_EULER_ANGLES );
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
						printf("Position %d Accelerometer--> %zX\n" , run, data_stream[run] );
					#endif
					run += 1;
					uint8_t_to_Point3( message.linear_acceleration , data_stream , run );
					run += 12 ;
				}
				else if( data_stream[run] == DataIMU::SCALED_GYRO_VECTOR ){
					#ifdef _DEBUG_SPILT_DATA_
						printf("Position %d SCALED GYRO --> %zX\n" , run ,data_stream[run]);
					#endif
					run += 1;
					uint8_t_to_Point3( message.angular_velocity , data_stream , run );
					run += 12 ;
				}
				else if( data_stream[ run ] == DataIMU::CF_EULER_ANGLES ){
					#ifdef _DEBUG_SPILT_DATA_
						printf("Position %d EULER ANGLES --> %zX\n" ,run ,data_stream[run]);
					#endif
					run += 1;
					uint8_t_to_Point3( message.euler , data_stream , run );
					run += 12;
				}
				else{
					#ifdef _DEBUG_SPILT_DATA_
						printf("Skip\n");
					#endif
					run += 1;
				}
			}
			tell_data.publish( message );	
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
