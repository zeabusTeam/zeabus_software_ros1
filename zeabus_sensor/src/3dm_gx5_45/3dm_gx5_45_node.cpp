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

// use pointer variable
	zeabus_sensor::MIP_COMMUNICATION::microstrain_imu_port* imu 
//		= new zeabus_sensor::MIP_COMMUNICATION::microstrain_imu_port();
		= new zeabus_sensor::MIP_COMMUNICATION::microstrain_imu_port("/dev/ttyACM0");
		
//	imu->set_name_port("/dev/ttyACM0");
	std::cout << "Open imu port\n";
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
	std::cout << "Set option of imu port\n";
	imu->io_port->set_option( imu->io_baud_rate);
	imu->io_port->set_option( imu->io_flow_control);
	imu->io_port->set_option( imu->io_parity);
	imu->io_port->set_option( imu->io_stop_bits);
	imu->io_port->set_option( imu->io_character_size);
	
	imu->set_idle();
	int imu_rate = imu->get_imu_data_base_rate();
	std::cout << "imu rate is " << imu_rate << "\n";
	int desired_base_rate = 250;
	uint16_t rate_decimation = uint16_t(imu_rate / desired_base_rate);
	std::cout << "rate_decimation is " << rate_decimation << "\n";
	
	// NEXT SET IMU MESSAGE FORMAT to CHOOSE WHAT DATA DO YOU WANT
	std::vector<uint8_t> imu_message_format; 
	imu_message_format.push_back(
			zeabus_sensor::MIP_COMMUNICATION::DATA::IMU_DATA_SET::DELTA_THETA_VECTOR
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

	// start part of ros
	ros::NodeHandle nh;

	ros::ServiceServer ser_cli_port_sensor = 
		nh.advertiseService("port_imu/status" , service_manage_port);	

	// SET PART of STREAM
	std::cout << "<---SYSTEM---> IMU STREAM DATA\n";
	std::vector<uint8_t> data;
	bool ok_data;
	while( nh.ok() && imu->is_open()){
		imu->stream_data( data , ok_data);
		if( ok_data ){
			std::cout << "<--IMU--> GOOD DATA\n";
		}
		else{
			std::cout << "<--IMU--> BAD DATA\n";
		}
		// this part for publisher data


		// the part of close port by service
		if( ! status_port ){
			imu->close_port();
		}
		ros::spinOnce();
	}

	delete imu;

}
