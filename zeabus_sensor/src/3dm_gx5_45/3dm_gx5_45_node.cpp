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

#ifndef ZEABUS_SENSOR_3DM_HX5_45_NODE_H
	#include	<zeabus_sensor/3dm_gx5_45_node.h>
	#define ZEABUS_SENSOR_3DM_HX5_45_NODE_H
#endif

int main( int argc , char **argv){

// init name node but if you have launch this node will name up to launch file
	ros::init( argc , argv, "node_imu");

// use pointer variable
	zeabus_sensor::MIP_COMMUNICATION::microstrain_imu_port* imu 
		= new zeabus_sensor::MIP_COMMUNICATION::microstrain_imu_port("/dev/ttyACM0");
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

	imu->io_port->set_option( imu->io_baud_rate);
	imu->io_port->set_option( imu->io_flow_control);
	imu->io_port->set_option( imu->io_parity);
	imu->io_port->set_option( imu->io_stop_bits);
	imu->io_port->set_option( imu->io_character_size);

}
