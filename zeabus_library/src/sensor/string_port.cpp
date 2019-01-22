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

//#define _DEBUG_READ_LINE_
//#define _DEBUG_INDIVIDUAL_READ_

namespace zeabus_library{

namespace sensor{

	StringPort::StringPort( std::string name_port ) : SerialPort( name_port ){}

	StringPort::~StringPort(){}

	size_t StringPort::read_data( std::string& message ){
		message = "";
		this->count = 0;
		#ifdef _DEBUG_INDIVIDUAL_READ_
			std::cout << "READ : ";
		#endif
		while( true ){
			this->buffer_size = boost::asio::read( this->io_port
												, boost::asio::buffer( &(this->temp) , 1 ) );
			if( this->temp == '\r' ){
				#ifdef _DEBUG_INDIVIDUAL_READ_
					std::cout << " <STARTCURSOR>" << "\n";
				#endif
				continue;
			}
			else if( this->temp == '\n' ){
				#ifdef _DEBUG_INDIVIDUAL_READ_
					std::cout << " <NEWLINE> " << "\n";
				#endif
				break;
			}
			else if( this->temp == '\0' ){
				#ifdef _DEBUG_INDIVIDUAL_READ_
					std::cout << " <ENDSTRING> " << "\n";
				#endif
			}
			else{
				message += this->temp; 
				#ifdef _DEBUG_INDIVIDUAL_READ_
					std::cout << this->temp << " ";
				#endif
				this->count++;
			}
		}
		#ifdef _DEBUG_INDIVIDUAL_READ_
				std::cout << "\n";
		#endif
		#ifdef _DEBUG_READ_LINE_
			std::cout << "OUTPUT : " << message << "\n" ;
		#endif
		return this->count; 
	}


	size_t StringPort::write_data( std::string& message ){
		this->buffer_size = boost::asio::write( this->io_port 
									, boost::asio::buffer( message )
									, boost::asio::transfer_all()
									, this->error_code );
		return this->buffer_size;	
	}

	size_t StringPort::write_data( std::string message ){
		this->buffer_size = boost::asio::write( this->io_port 
									, boost::asio::buffer( message )
									, boost::asio::transfer_all()
									, this->error_code );
		return this->buffer_size;	
	}
}
}
