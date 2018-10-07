/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File	: 3dm_gx5_45_node.cpp
//
//	Create	: Oct 07 , 2018
//	Author	: Supasan Komonlit
//
//	Main Purpose : for manage connection with IMU Lord microstrain 3dx_gx5_45 
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include	"3dm_gx5_45_node.h"

int main( int argc , char **argv){
	ros::init( argc , argv , "individual_imu"); // init node for ros system
	
	// set variable port
	imu_port = new zeabus_extension::manage_port::specific_port("/dev/ttyACM0");	

	imu_port->open_port();
	
	imu_port->io_baud_rate = boost::asio::serial_port_base::baud_rate( 115200 );

	imu_port->io_port->set_option( imu_port->io_baud_rate );
	std::cout	<< "Baud rate of IMU is " 
				<< imu_port->io_baud_rate.value()  << "\n";
	imu_port->io_port->get_option( imu_port->io_baud_rate );
	std::cout	<< "Baud rate of IMU is " 
				<< imu_port->io_baud_rate.value()  << "\n";

}
