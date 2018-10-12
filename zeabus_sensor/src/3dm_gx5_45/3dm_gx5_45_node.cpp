/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File	: 3dm_gx5_45_node.cpp
//
//	Create	: Oct 09 , 2018
//	Author	: Supasan Komonlit
//
//	Main Purpose : main node of LORD MICROSTRIN SENSOR 3dm_gx5_45
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include	<zeabus_sensor/3dm_gx5_45_node.h>

#include	"imu_port.cpp"
#include	"imu_command.cpp"

int main( int argc , char **argv){

// init name node but if you have launch this node will name up to launch file
	
	ros::init( argc , argv, "node_imu");
	
/////////////////////////////////////PART OF ROS LAUNCH//////////////////////////////////////////
//	use nh.param( name of param in launch , variable to collect value , default value)
	std::string name_node = ros::this_node::getName();
	ros::NodeHandle nh("");
	ros::NodeHandle ph("~");
	// this is service to close or open port of imu
	std::string imu_name_service_status;
	#ifdef TEST_ROS_SYSTEM
		if( ph.getParam("port_imu/name_service_status", imu_name_service_status)){
			std::cout << "<--SYSTEM-->" << name_node << " have get param" << "\n";
		}
		else{
			std::cout << "<--SYSTEM-->" << name_node << " don't have get param" << "\n";
		}
	#endif	
	ph.param("port_imu/name_service_status" , imu_name_service_status 
										, std::string("port_imu/status"));
	ros::ServiceServer ser_cli_port_sensor = 
		nh.advertiseService( imu_name_service_status , service_manage_port);	
	// this is part of topic for send velocity in angular and linear
	std::string imu_topic_velocity;
	ph.param("port_imu/topic_velocity" , imu_topic_velocity 
										, std::string("port_imu/velocity"));	
	ros::Publisher tell_velocity = nh.advertise<geometry_msgs::TwistStamped>(
										imu_topic_velocity, 1
									);
	// this is part of topic for send position of angle by use euler
	std::string imu_topic_euler;
	ph.param("port_imu/topic_euler" , imu_topic_euler , std::string("port_imu/euler"));	
	ros::Publisher tell_euler = nh.advertise<geometry_msgs::Vector3Stamped>(
										imu_topic_euler , 1 
									);
	// set message to for publisher
	geometry_msgs::Vector3Stamped euler_message;
	geometry_msgs::TwistStamped velocity_message;
	
//
///////////////// END PART PURE ROS NEXT PART GET VALUE FOR SERIAL PORT
//
// use pointer variable
	std::string name_port;
	ph.param("port_imu/name_port" , name_port , std::string("/dev/ttyACM0"));
	zeabus_sensor::MIP_COMMUNICATION::microstrain_imu_port* imu 
		= new zeabus_sensor::MIP_COMMUNICATION::microstrain_imu_port( name_port );
	// baud rate is rate for send symbol of message have learn in DATA COMMUNICATION
	int temporary_rate ;
	ph.param("port_imu/baud_rate" , temporary_rate , 11520);
	unsigned int imu_baud_rate = (unsigned int)temporary_rate;
	// THIS RATE TO USE WITH IMU AND RATE OF CODE
	int desired_base_rate;
	ph.param("port_imu/desired_base_rate" , desired_base_rate , 250);
	ros::Rate rate( desired_base_rate );
	

////////////////////////// PART ABOUT PORT CONNECT AND SET UP //////////////////////////////////
//
	imu->open_port();

	imu->io_baud_rate = boost::asio::serial_port_base::baud_rate((unsigned int) 115200);
	imu->io_flow_control = boost::asio::serial_port_base::flow_control(
								boost::asio::serial_port_base::flow_control::none
							);
	imu->io_parity = boost::asio::serial_port_base::parity(
						boost::asio::serial_port_base::parity::none
					);
	imu->io_stop_bits = boost::asio::serial_port_base::stop_bits(
						boost::asio::serial_port_base::stop_bits::one
					);
	imu->io_character_size = boost::asio::serial_port_base::character_size((size_t)8);
	// set option boost library use oeverload to manage this function
	imu->io_port->set_option( imu->io_baud_rate);
	imu->io_port->set_option( imu->io_flow_control);
	imu->io_port->set_option( imu->io_parity);
	imu->io_port->set_option( imu->io_stop_bits);
	imu->io_port->set_option( imu->io_character_size);

////////////// FINISH PART OF CONNECT AND SET UP IMU NEXT PART SET UP IMU

	imu->set_idle();
	int imu_rate = imu->get_imu_data_base_rate();
	uint16_t rate_decimation = uint16_t(imu_rate / desired_base_rate);
	
	// NEXT SET IMU MESSAGE FORMAT to CHOOSE WHAT DATA DO YOU WANT
	std::vector<uint8_t> imu_message_format; 
	imu_message_format.push_back(
			zeabus_sensor::MIP_COMMUNICATION::DATA::IMU_DATA_SET::SCALED_GYRO_VECTOR
		);
	imu_message_format.push_back(
			zeabus_sensor::MIP_COMMUNICATION::DATA::IMU_DATA_SET::DELTA_VELOCITY_VECTOR
		);
	imu_message_format.push_back(
			zeabus_sensor::MIP_COMMUNICATION::DATA::IMU_DATA_SET::CF_EULER_ANGLES
		);
	imu->set_imu_message_format_field( rate_decimation , imu_message_format );
	imu->set_imu_message_format();
	// finish part set IMU MESSAGE FORMAT

	std::cout << "will Enable stream\n";
	// true is enable stream and first order is imu and second is ef	
	imu->continuous_stream( true , false);
//
///////////////////////////////////////////////////////////////////////////////////////////////
	// SET PART of STREAM
	std::cout << "<---SYSTEM---> IMU STREAM DATA\n";
	std::vector<uint8_t> data;
	int32_t temporary;
	float message;
	bool ok_data;
	while( nh.ok() && imu->is_open()){
		rate.sleep();
		imu->stream_data( data , ok_data);
		if( ok_data ){
			std::cout << "<--IMU--> GOOD DATA\n";
			// this part for consider and get value to message
			for( int run = 0 ; run < data.size() ; ){
				// read length data for each set 1 bytes
				run++;
				// if data is of delta_theta_vector
				if(data[run] == 
					zeabus_sensor::MIP_COMMUNICATION::DATA::IMU_DATA_SET::SCALED_GYRO_VECTOR){
					// finish read data descriptor field
					run++;
					temporary = ( int32_t(data[run]) << 24 ) + (int32_t(data[run+1] << 16 ) ) 
								+ ( int32_t(data[run+2]) << 8 ) + (int32_t(data[run+3]) << 0);
					memcpy( &message , &temporary , 4);
					velocity_message.twist.angular.x = message;
					run+=4;
					temporary = ( int32_t(data[run]) << 24 ) + (int32_t(data[run+1] << 16 ) ) 
								+ ( int32_t(data[run+2]) << 8 ) + (int32_t(data[run+3]) << 0);
					memcpy( &message , &temporary , 4);	
					velocity_message.twist.angular.y = message;
					run+=4;	
					temporary = ( int32_t(data[run]) << 24 ) + (int32_t(data[run+1] << 16 ) ) 
								+ ( int32_t(data[run+2]) << 8 ) + (int32_t(data[run+3]) << 0);
					memcpy( &message , &temporary , 4);	
					velocity_message.twist.angular.z = message;
					run+=4;	
					//	skip to read length data for each set 1 bytes
				}
				else if(data[run] ==
					zeabus_sensor::MIP_COMMUNICATION::DATA::IMU_DATA_SET::DELTA_VELOCITY_VECTOR){
					run++; // finish read data descriptor field
					temporary  = ( int32_t(data[run]) << 24 ) + (int32_t(data[run+1] << 16 ) ) 
								+ ( int32_t(data[run+2]) << 8 ) + (int32_t(data[run+3]) << 0);
					memcpy( &message , &temporary , 4);	
					velocity_message.twist.linear.x = message * standard_gravity_constant;
					run+=4;
					temporary  = ( int32_t(data[run]) << 24 ) + (int32_t(data[run+1] << 16 ) ) 
								+ ( int32_t(data[run+2]) << 8 ) + (int32_t(data[run+3]) << 0);
					memcpy( &message , &temporary , 4);	
					velocity_message.twist.linear.y = message * standard_gravity_constant;
					run+=4;
					temporary = ( int32_t(data[run]) << 24 ) + (int32_t(data[run+1] << 16 ) ) 
								+ ( int32_t(data[run+2]) << 8 ) + (int32_t(data[run+3]) << 0);
					memcpy( &message , &temporary , 4);	
					velocity_message.twist.linear.z = message * standard_gravity_constant;
					run+=4;
					// skip to read length data for each set 1 bytes
				}
				else if(data[run] ==
					zeabus_sensor::MIP_COMMUNICATION::DATA::IMU_DATA_SET::CF_EULER_ANGLES){
					run++;
					temporary  = ( int32_t(data[run]) << 24 ) + (int32_t(data[run+1] << 16 ) ) 
								+ ( int32_t(data[run+2]) << 8 ) + (int32_t(data[run+3]) << 0);
					memcpy( &message , &temporary , 4);
					euler_message.vector.x = message;	
					run+=4;
					temporary  = ( int32_t(data[run]) << 24 ) + (int32_t(data[run+1] << 16 ) ) 
								+ ( int32_t(data[run+2]) << 8 ) + (int32_t(data[run+3]) << 0);
					memcpy( &message , &temporary , 4);	
					euler_message.vector.y = message;	
					run+=4;
					temporary  = ( int32_t(data[run]) << 24 ) + (int32_t(data[run+1] << 16 ) ) 
								+ ( int32_t(data[run+2]) << 8 ) + (int32_t(data[run+3]) << 0);
					memcpy( &message , &temporary , 4);	
					euler_message.vector.z = message;
					run+=4;
					// skip to rad length data for each set 1 bytes
				}
			}
			// this part for publisher data
			euler_message.header.stamp = ros::Time::now();
			velocity_message.header.stamp = ros::Time::now();
			tell_euler.publish( euler_message );
			tell_velocity.publish( velocity_message );
		}
		else{
			std::cout << "<--IMU--> BAD DATA\n";
		}
		// the part of close port by service
		if( ! status_port ){
//			imu->continuous_stream( false , false);
			break;
		}
		rate.sleep();
		ros::spinOnce();
	}
	
	imu->continuous_stream( false , false);
	delete imu;

}
