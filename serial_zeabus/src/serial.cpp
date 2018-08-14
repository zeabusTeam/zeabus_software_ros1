/////////////////////////////////////////////////////////////////////////////////

//	serial.cpp
//  Created on : Aug 11 , 2018
//      Author : Supasan
//Follow original of
//  Created on : Nov 17 , 2014
//      Author : Mahisorn
//
/////////////////////////////////////////////////////////////////////////////////

#include	<serial_zeabus/serial.h>

serial_zeabus::serial::serial() : 
	IO(), port(IO), timer(IO), byte_transferred(0), IO_state(IO_done){}

serial_zeabus::~serial(){}

bool serial_zeabus::serial::open_port(	std::string name_port , 
				unsigned int baud_rate, 
				boost::asio::serial_port_base::parity opt_parity =
					boost::asio::serial_port_base::parity( 
						boost::asio::serial_port_base::parity::none),

				boost::asio::serial_port_base::character_size opt_csize = 
					boost::asio::serial_port_base::character_size(8),

				boost::asio::serial_port_base::flow_control opt_flow =
					boost::asio::serial_port_base::flow_control(
						boost::asio::serial_port::flow_control::none),

				boost::asio::serial_port_base::stop_bits opt_stop = 
					boost::asio::serial_port_base::stop_bits(
						boost::asio::serial_port_base::stop_bits::one)
				){
	if( is_open() )	
		serial_zeabus::serial::close_port();

	try{
		port.open( name_port );
		port.set_option( boost::asio::serial_port_base::baud_rate( baud_rate ));
		port.set_option( opt_parity );
		port.set_option( opt_csize );
		port.set_option( opt_flow );
		port.set_option( opt_stop );
	}
	catch (boost::system::system_error& message){
		std::cout << "Error: " << e.what() << "\n";
		return false;
	}
	return true;
}

void serial_zeabus::serial::close_port(){
	port.close();
}

bool serial_zeabus::serial::is_open(){
	return port.is_open();
}

void serial_zeabus::serial::change_baud_rate( unsigned int baud_rate){
	try{
		port.set_option( boost::asio::serial_port_base::baud_rate( baud_rate) );
	}
	catch (boost::system::system_error& message){
		std::cout << "Error: " << message.what() << "\n";
	}
}

void serial_zeabus::serial::write_package( const serial_zeabus::package& data , int timeout){
	data_stream	temporary;
	data.get_data_stream( temporary );
	serial_zeabus::serial::wirte_data( temporary , timeout);
}


