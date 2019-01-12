/*
	File name			:	string_port.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2018 , NOV 28
	Date last modified	:	2018 , DEC 05
	Purpose				:	This is header of file abount port style synchronous

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Maintain & Using
							
	Namespace			:	zeabus_library/sensor
*/

#include	<zeabus_library/sensor/string_port.h>

//#define		_DEBUG_CONNECTION_

namespace zeabus_library{

namespace sensor{

	StringPort::StringPort( std::string name_port ) : SerialPort( name_port ){}

	StringPort::~StringPort(){}

	size_t StringPort::read_data( std::string& message ){
		message = "";
		this->count = 0;
		while( true ){
			this->buffer_size = boost::asio::read( this->io_port
												, boost::asio::buffer( &(this->temp) , 1 ) );
			if( this->temp == '\r' ) break;
			else if( this->temp == '\n' ) break;
			else{
				message += this->temp; 
				this->count++;
			}
		}
		return this->count; 
	}


	size_t StringPort::write_data( std::string& message ){
		this->buffer_size = boost::asio::write( this->io_port 
									, boost::asio::buffer( message )
									, boost::asio::transfer_all()
									, this->error_code );
		return this->buffer_size;	
	}

}
}
