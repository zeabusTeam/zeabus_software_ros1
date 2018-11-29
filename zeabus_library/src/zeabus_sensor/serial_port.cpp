/*
	File name			:	serial_port.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2018 , NOV 26
	Date last modified	:	2018 , ??? ??
	Purpose				:	This is source code of file about standard doing to do with port

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.0.1
	status				:	Production

	Namespace			:	zeabus_sensor
*/

#include	<zeabus_library/zeabus_sensor/serial_port.h>

namespace zeabus_sensor{
	
	SerialPort::SerialPort( std::string name_port ){
		this->name_port = name_port;
	}

	SerialPort::~SerialPort(){
		this->is_open( &(this->temp_boolean) );
		if( this->temp_boolean ){
			printf( "<==== ZEABUS SENSOR ====> CLOSE PORT\n");
			this.close_port( &(this->temp_boolean) );
		}
		else{
			printf( "<==== ZEABUS SENSOR ====> PORT DOESN\'T OPEN\n");
		}
	}

	void SerialPort::open_port( bool& result ){
		try{
			this->io_port.open( this->name_port );
		}
		catch( const std::exception& error ){
			printf( "<==== ZEABUS SENSOR ====> Error open port < " + this->name_port + " >\n");
		}
	}

	void SerialPort::is_open( bool& result ){
		result = this->io_port.is_open();
	}

	void SerialPort::close_port( bool& result ){
		this->is_open( &( this->temp_boolean ) );
		if( this->temp_boolean ){
			this->io_port.close( &( this->error_code ) );
			if( this->error_code == errc::success ){
				printf("<==== ZEABUS SENSOR ====> Success close port" + this->name_port + "\n");
			}	
		}
		else{
			printf( "<==== ZEABUS SENSOR ====> PORT DOESN\'T OPEN\n");
		}
	}
			
	void SerialPort::set_option_port( boost::asio::serial_port_base::baud_rate baud_rate){
		this->io_port.set_option( baud_rate , &( this->error_code ) );
		if( this->error_code != errc::success ){
			printf("<==== ZEABUS SENSOR ====> Failure set baud_rate of "
						+ this->name_port + "\n" );
		}
	}

	void SerialPort::set_option_port( boost::asio::serial_port_base::flow_control flow_control){
		this->io_port.set_option( flow_control , &( this->error_code ) );
		if( this->error_code != errc::success ){
			printf("<==== ZEABUS SENSOR ====> Failure set flow_control of "
						+ this->name_port + "\n" );
		}
	}

	void SerialPort::set_option_port( boost::asio::serial_port_base::parity parity){
		this->io_port.set_option( parity , &( this->error_code ) );
		if( this->error_code != errc::success ){
			printf("<==== ZEABUS SENSOR ====> Failure set parity of "
						+ this->name_port + "\n" );
		}
	}
	
	void SerialPort::set_option_port( boost::asio::serial_port_base::stop_bits stop_bits){
		this->io_port.set_option( stop_bits , &( this->error_code ) );
		if( this->error_code != errc::success ){
			printf("<==== ZEABUS SENSOR ====> Failure set stop_bits of "
						+ this->name_port + "\n" );
		}
	}
	
	void SerialPort::set_option_port( 
						boost::asio::serial_port_base::character_size character_size){
		this->io_port.set_option( character_size , &( this->error_code ) );
		if( this->error_code != errc::success ){
			printf("<==== ZEABUS SENSOR ====> Failure set character_size of "
						+ this->name_port + "\n" );
		}
	}

	void SerialPort::get_option_port( boost::asio::serial_port_base::baud_rate& baud_rate){
		this->io_port.get_option( baud_rate , &( this->error_code ) );
		if( this->error_code != errc::success ){
			printf("<==== ZEABUS SENSOR ====> Failure set baud_rate of "
						+ this->name_port + "\n" );
		}
	}

	void SerialPort::get_option_port( boost::asio::serial_port_base::flow_control& flow_control){
		this->io_port.get_option( flow_control , &( this->error_code ) );
		if( this->error_code != errc::success ){
			printf("<==== ZEABUS SENSOR ====> Failure set flow_control of "
						+ this->name_port + "\n" );
		}
	}

	void SerialPort::get_option_port( boost::asio::serial_port_base::parity& parity){
		this->io_port.get_option( parity , &( this->error_code ) );
		if( this->error_code != errc::success ){
			printf("<==== ZEABUS SENSOR ====> Failure set parity of "
						+ this->name_port + "\n" );
		}
	}

	void SerialPort::get_option_port( boost::asio::serial_port_base::stop_bits& stop_bits){
		this->io_port.get_option( stop_bits , &( this->error_code ) );
		if( this->error_code != errc::success ){
			printf("<==== ZEABUS SENSOR ====> Failure set stop_bits of "
						+ this->name_port + "\n" );
		}
	}

	void SerialPort::get_option_port( 
						boost::asio::serial_port_base::character_size& character_size){
		this->io_port.get_option( character_size , &( this->error_code ) );
		if( this->error_code != errc::success ){
			printf("<==== ZEABUS SENSOR ====> Failure set character_size of "
						+ this->name_port + "\n" );
		}
	}
}
