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
	// set variable for set baudrate	
	imu_port->io_baud_rate = boost::asio::serial_port_base::baud_rate( 115200 );
	// set variable for set flow_control	
	imu_port->io_flow_control = 
		boost::asio::serial_port_base::flow_control( 
			boost::asio::serial_port_base::flow_control::none ) ;
	// set variable for set parity	
	imu_port->io_parity =
		boost::asio::serial_port_base::parity( 
			boost::asio::serial_port_base::parity::none );
	// set variable for set stop_bits	
	imu_port->io_stop_bits = 
		boost::asio::serial_port_base::stop_bits(
			boost::asio::serial_port_base::stop_bits::one);
	// set variable for set character_size	
	imu_port->io_character_size = 
		boost::asio::serial_port_base::character_size(8);

	// set up all option
	if( ! imu_port->is_open() ) return-1;
	imu_port->io_port->set_option( imu_port->io_baud_rate );
	imu_port->io_port->set_option( imu_port->io_stop_bits );
	imu_port->io_port->set_option( imu_port->io_parity );
	imu_port->io_port->set_option( imu_port->io_flow_control );
	imu_port->io_port->set_option( imu_port->io_character_size );
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////


	delete imu_port;

}
