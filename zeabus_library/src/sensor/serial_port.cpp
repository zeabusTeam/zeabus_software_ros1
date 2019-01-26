/*
	File name			:	serial_port.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2018 , NOV 26
	Date last modified	:	2018 , NOV 28
	Purpose				:	This is source code of file about standard doing to do with port

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.1.1
	status				:	Maintain & Using

	Namespace			:	zeabus_library/sensor
*/

#include	<zeabus_library/sensor/serial_port.h>

namespace zeabus_library{

namespace sensor{
	
	SerialPort::SerialPort( std::string name_port ) : io_port( this->io_service ){
		this->name_port = name_port;
	}

	SerialPort::~SerialPort(){
		this->is_open( this->temp_boolean );
		if( this->temp_boolean ){
			this->close_port( this->temp_boolean );
		}
	}

	void SerialPort::open_port( bool& result ){
		try{
			this->io_port.open( this->name_port );
			printf( "Port < %s > now open\n" , this->name_port.c_str() );
			result = true;
		}
		catch( const std::exception& error ){
			result = false;
			printf( "<==== WARNING ====> Error open port < %s >\n"
							, this->name_port.c_str());
		}
	}

	void SerialPort::is_open( bool& result ){
		result = this->io_port.is_open();
	}

	void SerialPort::close_port( bool& result ){
		this->is_open( this->temp_boolean );
		if( this->temp_boolean ){
			this->io_port.close( this->error_code );
			if( this->error_code != errc::success ){
				printf("<==== WARNING ====> Error close port < %s >\n"
							, this->name_port.c_str() );
			}
			else{
				printf("Port < %s > now close\n" , this->name_port.c_str() );
			}	
		}
		else{
			printf( "<==== GENERAL ====> PORT DOESN\'T OPEN\n");
		}
	}
			
	void SerialPort::set_option_port( boost::asio::serial_port_base::baud_rate baud_rate){
		this->io_port.set_option( baud_rate , this->error_code );
		if( this->error_code != errc::success ){
			printf("<==== WARNING ====> Failure set baud_rate of %s\n"
						, this->name_port.c_str() );
		}
	}

	void SerialPort::set_option_port( boost::asio::serial_port_base::flow_control flow_control){
		this->io_port.set_option( flow_control , this->error_code  );
		if( this->error_code != errc::success ){
			printf("<==== WARNING ====> Failure set flow_control of %s\n"
						, this->name_port.c_str() );
		}
	}

	void SerialPort::set_option_port( boost::asio::serial_port_base::parity parity){
		this->io_port.set_option( parity , this->error_code  );
		if( this->error_code != errc::success ){
			printf("<==== WARNING ====> Failure set parity of %s\n"
						, this->name_port.c_str() );
		}
	}
	
	void SerialPort::set_option_port( boost::asio::serial_port_base::stop_bits stop_bits){
		this->io_port.set_option( stop_bits , this->error_code  );
		if( this->error_code != errc::success ){
			printf("<==== WARNING ====> Failure set stop_bits of %s\n"
						, this->name_port.c_str() );
		}
	}
	
	void SerialPort::set_option_port( 
						boost::asio::serial_port_base::character_size character_size){
		this->io_port.set_option( character_size , this->error_code );
		if( this->error_code != errc::success ){
			printf("<==== WARNING ====> Failure set character_size of %s\n"
						, this->name_port.c_str() );
		}
	}

	void SerialPort::get_option_port( boost::asio::serial_port_base::baud_rate& baud_rate){
		this->io_port.get_option( baud_rate , this->error_code );
		if( this->error_code != errc::success ){
			printf("<==== WARNING ====> Failure set baud_rate of %s\n"
						, this->name_port.c_str() );
		}
	}

	void SerialPort::get_option_port( boost::asio::serial_port_base::flow_control& flow_control){
		this->io_port.get_option( flow_control , this->error_code );
		if( this->error_code != errc::success ){
			printf("<==== WARNING ====> Failure set flow_control of %s\n"
						, this->name_port.c_str() );
		}
	}

	void SerialPort::get_option_port( boost::asio::serial_port_base::parity& parity){
		this->io_port.get_option( parity , this->error_code );
		if( this->error_code != errc::success ){
			printf("<==== WARNING ====> Failure set parity of %s\n"
						, this->name_port.c_str() );
		}
	}

	void SerialPort::get_option_port( boost::asio::serial_port_base::stop_bits& stop_bits){
		this->io_port.get_option( stop_bits , this->error_code );
		if( this->error_code != errc::success ){
			printf("<==== WARNING ====> Failure set stop_bits of %s\n"
						, this->name_port.c_str() );
		}
	}

	void SerialPort::get_option_port( 
						boost::asio::serial_port_base::character_size& character_size){
		this->io_port.get_option( character_size , this->error_code );
		if( this->error_code != errc::success ){
			printf("<==== WARNING ====> Failure set character_size of %s\n"
						, this->name_port.c_str() );
		}
	}
}
}
