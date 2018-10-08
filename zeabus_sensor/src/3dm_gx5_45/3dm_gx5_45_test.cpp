/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File	: 3dm_gx5_45_test.cpp
//
//	Create	: Oct 07 , 2018
//	Author	: Supasan Komonlit
//
//	Main Purpose : for manage connection with IMU Lord microstrain 3dx_gx5_45 
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include	"3dm_gx5_45_node.h"

#define TEST_IMU

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
///////////////////////////////// PART TEST IMU //////////////////////////////////////////////
	std::vector<unsigned char>idle_command;
	
	std::string idle_string = "756501020202E1C7";
//	std::string idle_string = "756501020201E0C6";
	for( int i = 0 ; i < 16 ; i++) idle_command.push_back( (unsigned char)idle_string[i]);
	std::cout << "print vector : ";
	for( std::vector<unsigned char>::iterator it = idle_command.begin() 
			; it != idle_command.end() ; ++it){
		//std::cout << std::hex << *it;
		printf("%c" , *it);
	}
		
	std::cout << "\n";

	imu_port->write_asynchronous( idle_command , size_t(16) );
	std::cout << "FINISH send idle command next read command\n";
	std::vector<unsigned char>idle_reply; 
	std::cout << "START TO find\n";
	while( true ){
		idle_reply.resize(1);
		idle_reply = imu_port->read_asynchronous( size_t(1));
		printf("\'%c\'  " , idle_reply[0] );
		if( idle_reply[0] == 0x75){
			break;
		}
	}
	std::cout << "\n";	
	while( true ){
		idle_reply.resize(1);
		idle_reply = imu_port->read_asynchronous( size_t(1));
		printf("\'%c\'  " , idle_reply[0] );
		if( idle_reply[0] == 'e'){
			break;
		}
	}
	std::cout << "\n";	
//	std::cout	<< "result is : ";
//	for( std::vector<unsigned char>::iterator it = idle_reply.begin() 
//		; it != idle_reply.end() ; ++it) printf( "%c\n" , *it);
//	std::cout	<< " : finish\n"; 

///////////////////////////////////////////////////////////////////////////////////////////////

	#ifdef TEST_IMU
		std::cout	<< "<-SYSTEM-> Befor delete imu_port"
					<< "\n";
	#endif
	delete imu_port;

}
