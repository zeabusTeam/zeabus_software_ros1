/*
	File name			:	imu.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2018 , FEB 06
	Date last modified	:	2018 , FEB 09
	Purpose				:	Connection between IMU with ROS system

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Maintainance

	Namespace			:	None
*/
//=====================>

//#define _TEST_CONNECTION_ // If define this line. This code willn't connect IMU hardware

#define	NED_TO_ENU

#include	<ros/ros.h>

#include	<sensor_msgs/Imu.h>

#include	<zeabus_library/sensor/lord_microstrain.h>
#include	<zeabus_library/convert/bytes.h>

#include	<tf/transform_broadcaster.h>

#include	<zeabus_library/tf_handle/tf_quaternion.h>

#include	<zeabus_library/subscriber/SubImu.h>


//#define	_DEBUG_SPILT_DATA_ // For debug about get value each part of packet

//#define _PUBLISH_DATA_ 

namespace Asio = boost::asio;
namespace DataIMU = zeabus_library::sensor::MIP_COMMUNICATION::DATA::IMU_DATA_SET ;

int main( int argv , char** argc ){
	
	ros::init( argv , argc , "imu");

	ros::NodeHandle ph("~"); // Handle for manage param from launch
	ros::NodeHandle nh(""); // Handle for manage about this file in ros system

//=====================> PARAMETER PART

	std::string publish_topic;
	std::string frame_id;
	std::string parent_id;
#ifdef _TEST_CONNECTION_ 
	std::string subscribe_topic;
#endif

	double offset_rotation[3];
	double offset_translation[3];
	double offset_value[3];
	bool use_offset;
	

	std::string port_name; 
	int frequency;

	ph.param< std::string >( "publish_topic" , publish_topic , "/sensor/imu");
#ifdef _TEST_CONNECTION_
	ph.param< std::string >( "subscribe_topic" , subscribe_topic , "/test/imu");
#endif
	ph.param< std::string >( "frame_id" ,  frame_id , "imu");
	ph.param< std::string >( "parent_id" ,  parent_id , "robot");

	ph.param< double >( "rotation_x" , offset_rotation[0] , 0.0 ); // roll
	ph.param< double >( "rotation_y" , offset_rotation[1] , 0.0 ); // pitch
	ph.param< double >( "rotation_z" , offset_rotation[2] , 0.0 ); // yaw
	ph.param< double >( "translation_x" , offset_translation[0] , 0.0 );
	ph.param< double >( "translation_y" , offset_translation[1] , 0.0 );
	ph.param< double >( "translation_z" , offset_translation[2] , 0.0 );
	ph.param< bool >( "use_offset" , use_offset , false );
	ph.param< double >( "offset_roll" , offset_value[0] , 0.0 );
	ph.param< double >( "offset_pitch" , offset_value[1] , 0.0 );
	ph.param< double >( "offset_yaw" , offset_value[2] , 0.0 );

	ph.param< int >("frequency" , frequency , 50 );
	ph.param< std::string >( "port_imu", port_name
			, "/dev/microstrain/3dm_gx5_45_0000__6251.65903");

//=====================> TRANSFORM PART

	static tf::TransformBroadcaster broadcaster;
	
	zeabus_library::tf_handle::TFQuaternion tf_quaternion;
	tf_quaternion.setEulerZYX( offset_rotation[2] , offset_rotation[1] , offset_rotation[0] );
	tf_quaternion.normalize();

	tf::Transform transform;
	transform.setOrigin( 
			tf::Vector3( offset_translation[0] , offset_translation[1] , offset_rotation[2]) );
	transform.setRotation( tf_quaternion );

//=====================> SET UP ROS SYSTEM

	ros::Time time;

	ros::Rate rate( frequency );

	sensor_msgs::Imu sensor; //  for collecting message come form sensor

	sensor.header.frame_id = frame_id;

	ros::Publisher pub_sensor = nh.advertise< sensor_msgs::Imu >( publish_topic , 1 );

#ifdef _TEST_CONNECTION_
	zeabus_library::subscriber::SubImu listener( &sensor );
	int received = 1;
	listener.register_ttl( &received , 1 );
	ros::Subscriber sub_sensor = nh.subscribe( subscribe_topic , 1 
			, &zeabus_library::subscriber::SubImu::callback_ttl
			, &listener );
	sensor.orientation.x = 0;
	sensor.orientation.y = 0;
	sensor.orientation.z = 0;
	sensor.orientation.w = 1;
#endif

#ifndef _TEST_CONNECTION_
//=====================> CONNECTION IMU PART

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

//=====================> END PART CONNECTION <SET UP> IMU
#endif
	std::vector< uint8_t > data_stream;
//=====================> LOOP ROS SYSTEM
	while( nh.ok() ){
		rate.sleep();

#ifndef _TEST_CONNECTION_

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
			if( use_offset ){
				zeabus_library::tf_handle::TFQuaternion temp_quaternion( sensor.orientation.x
						, sensor.orientation.y , sensor.orientation.z , sensor.orientation.w );
				double roll , pitch , yaw ;
				temp_quaternion.get_RPY( roll,  pitch , yaw);
				roll += offset_value[0];
				pitch += offset_value[1];
				yaw += offset_value[2];
				zeabus_library::tf_handle::edit_value( roll );
				zeabus_library::tf_handle::edit_value( pitch );
				zeabus_library::tf_handle::edit_value( yaw );
				temp_quaternion.setEulerZYX( yaw , pitch , roll );
				sensor.orientation.x = temp_quaternion.x();
				sensor.orientation.y = temp_quaternion.y();
				sensor.orientation.z = temp_quaternion.z();
				sensor.orientation.w = temp_quaternion.w();
			}
			time = ros::Time::now();
			sensor.header.stamp = time;
			pub_sensor.publish( sensor );
			broadcaster.sendTransform( 
					tf::StampedTransform( transform , time , parent_id , frame_id ) );
		}

#else
		rate.sleep();
		ros::spinOnce();
		if( use_offset && received ){
			zeabus_library::tf_handle::TFQuaternion temp_quaternion( sensor.orientation.x
					, sensor.orientation.y , sensor.orientation.z , sensor.orientation.w );
			double roll , pitch , yaw ;
			temp_quaternion.get_RPY( roll,  pitch , yaw);
			roll += offset_value[0];
			pitch += offset_value[1];
			yaw += offset_value[2];
			zeabus_library::tf_handle::edit_value( roll );
			zeabus_library::tf_handle::edit_value( pitch );
			zeabus_library::tf_handle::edit_value( yaw );
			temp_quaternion.setEulerZYX( yaw , pitch , roll );
			sensor.orientation.x = temp_quaternion.x();
			sensor.orientation.y = temp_quaternion.y();
			sensor.orientation.z = temp_quaternion.z();
			sensor.orientation.w = temp_quaternion.w();
			received = 0;
		}
		time = ros::Time::now();
		sensor.header.stamp = time;
		sensor.header.frame_id = frame_id;
		pub_sensor.publish( sensor );
		broadcaster.sendTransform( tf::StampedTransform( transform, time, parent_id, frame_id ));
#endif

	}	

	int count = 0;
//=====================> Close port IMU set to IDLE MODE
#ifndef _TEST_CONNECTION_
	do{
		printf( "IMU set to ide ");
		imu.command_idle( result );
		rate.sleep();
		count++;
	}while( ( ! result ) && count < 10 );
#endif

}
