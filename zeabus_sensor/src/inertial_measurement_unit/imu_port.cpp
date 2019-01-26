/*
	File name			:	imu_port.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2018 , NOV 27
	Date last modified	:	2018 , DEC 27
	Purpose				:	This is file to use read connect ros and IMU

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.2.1
	status				:	Maintain

	Namespace			:	None
*/

#include	<ros/ros.h>

#include	<zeabus_library/IMUQuaternion.h>
#include	<zeabus_library/IMUData.h>
#include	<sensor_msgs/Imu.h>

#include	<zeabus_library/sensor/lord_microstrain.h>
#include	<zeabus_library/convert/bytes.h>

//#define	_DEBUG_SPILT_DATA_
//#define _TYPE_SENSOR_MSGS_
//#define _TYPE_ZEABUS_LIBRARY_MSGS_
#define _TYPE_ZEABUS_LIBRARY_QUATERNION_

namespace Asio = boost::asio;
namespace DataIMU = zeabus_library::sensor::MIP_COMMUNICATION::DATA::IMU_DATA_SET ;

int main( int argv , char** argc ){
	
	ros::init( argv , argc , "imu_port");

	ros::NodeHandle ph("~"); // Handle for manage param from launch
	ros::NodeHandle nh(""); // Handle for manage about this file in ros system

////////////////////////////////////-- PARAMETER PART --/////////////////////////////////////////
	std::string topic_output_zeabus;
	std::string topic_output_sensor;
	std::string port_name;
	int frequency;

	ph.param< std::string >("name_port_imu" , port_name 
								, "/dev/microstrain/3dm_gx5_45_0000__6251.65903");

	ph.param< std::string >(	"topic_output_port_imu_zeabus" 
								, topic_output_zeabus , "/sensor/imu/port/zeabus");
	ph.param< std::string >(	"topic_output_port_imu_sensor" 
								, topic_output_sensor , "/sensor/imu/port/sensor");

	ph.param< int >("frequency_imu" , frequency , 50 );

////////////////////////////////////////////////////////////////////////////////////////////////


	ros::Rate rate( frequency );

	#ifdef _TYPE_ZEABUS_LIBRARY_MSGS_
		ros::Publisher tell_zeabus_library = 
				ph.advertise< zeabus_library::IMUData >( topic_output_zeabus, 1 );
		zeabus_library::IMUData message;
	#endif

	#ifdef _TYPE_ZEABUS_LIBRARY_QUATERNION_
		ros::Publisher tell_zeabus_library = 
				ph.advertise< zeabus_library::IMUQuaternion >( topic_output_zeabus, 1 );
		zeabus_library::IMUQuaternion imu_quaternion;
	#endif

	#ifdef _TYPE_SENSOR_MSGS_
		ros::Publisher tell_sensor_msgs =
				ph.advertise< sensor_msgs::Imu >( topic_output_sensor , 1 );
		sensor_msgs::Imu sensor;
	#endif

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
		#ifdef _TYPE_ZEABUS_LIBRARY_MSGS_
			imu.sensor_add_message_type( DataIMU::CF_EULER_ANGLES );
		#endif
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
					#ifdef _TYPE_ZEABUS_LIBRARY_MSGS_
						zeabus_library::convert::uint8_t_to_Point3(	
															message.linear_acceleration 
															, data_stream , run );
					#endif
					#ifdef _TYPE_SENSOR_MSGS_
						zeabus_library::convert::uint8_t_to_Vector3( 
															sensor.linear_acceleration
															, data_stream , run );
					#endif
					#ifdef _TYPE_ZEABUS_LIBRARY_QUATERNION_
						zeabus_library::convert::uint8_t_to_Point3( 
															imu_quaternion.linear_acceleration
															, data_stream , run );
					#endif
					run += 12 ;
				}
				else if( data_stream[run] == DataIMU::SCALED_GYRO_VECTOR ){
					#ifdef _DEBUG_SPILT_DATA_
						printf("Position %d SCALED GYRO --> %2X\n" , run ,data_stream[run]);
					#endif
					run += 1;
					#ifdef _TYPE_ZEABUS_LIBRARY_MSGS_
						zeabus_library::convert::uint8_t_to_Point3(	
															message.angular_velocity 
															, data_stream , run );
					#endif
					#ifdef _TYPE_SENSOR_MSGS_
						zeabus_library::convert::uint8_t_to_Vector3( 
															sensor.angular_velocity
															, data_stream , run );
					#endif
					#ifdef _TYPE_ZEABUS_LIBRARY_QUATERNION_
						zeabus_library::convert::uint8_t_to_Point3( 
															imu_quaternion.angular_velocity
															, data_stream , run );
					#endif
					run += 12 ;
				}
				else if( data_stream[ run ] == DataIMU::CF_EULER_ANGLES ){
					#ifdef _DEBUG_SPILT_DATA_
						printf("Position %d EULER ANGLES --> %2X\n" ,run ,data_stream[run]);
					#endif
					run += 1;
					#ifdef _TYPE_ZEABUS_LIBRARY_MSGS_
						zeabus_library::convert::uint8_t_to_Point3( 
															message.euler 
															, data_stream , run );
					#endif
					run += 12;
				}
				else if( data_stream[ run ] == DataIMU::CF_QUATERNION ){
					#ifdef _DEBUG_SPILT_DATA_
						printf("Position %d QUATERNION --> %2X\n" ,run ,data_stream[run]);
					#endif
					run += 1;
					#ifdef _TYPE_SENSOR_MSGS_
						zeabus_library::convert::uint8_t_to_Quaternion( sensor.orientation
																, data_stream , run );
					#endif
					#ifdef _TYPE_ZEABUS_LIBRARY_QUATERNION_
						zeabus_library::convert::uint8_t_to_Point4( imu_quaternion.quaternion
															, data_stream , run );
					#endif
					run += 16;
				}
				else{
					#ifdef _DEBUG_SPILT_DATA_
						printf("Skip\n");
					#endif
					run += 1;
				}
			}
			#ifdef _TYPE_ZEABUS_LIBRARY_MSGS_
				tell_zeabus_library.publish( message );	
			#endif
			#ifdef _TYPE_ZEABUS_LIBRARY_QUATERNION_
				tell_zeabus_library.publish( imu_quaternion );	
			#endif
			#ifdef _TYPE_SENSOR_MSGS_
				tell_sensor_msgs.publish( sensor );
			#endif
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
