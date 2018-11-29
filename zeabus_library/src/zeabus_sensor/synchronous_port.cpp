/*
	File name			:	synchronous_port.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2018 , NOV 28
	Date last modified	:	2018 , ??? ??
	Purpose				:	This is header of file abount port style synchronous

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.0.1
	status				:	Prouction
							
	Namespace			:	zeabus_sensor
*/

#include	<zeabus_library/zeabus_sensor/synchronous_port.h>

#define		_DEBUG_CONNECTION_

namespace zeabus_sensor{

	SynchroPort::SynchroPort( std::string name_port ) : SerialPort( name_port ){}

	SynchroPort::~SynchroPort(){}

	size_t SynchroPort::read_data( std::vector<uint8_t>& buffer , size_t& size ){
		if( buffer.size() < size ){
			buffer.resize( size ); // must ensure your buffer have size can collect data
		}
		#ifdef _DEBUG_CONNECTION_
			printf( "want read size < %zd > ===" , size);
		#endif
			this->buffer_size = boost::asio::read(	this->io_port 
													, boost::asio::buffer( buffer , size ) 
													, this->error_code );
		#ifdef _DEBUG_CONNECTION_
			printf( "--- < %zd >\n" , this->buffer_size);
		#endif
			if( this->error_code != errc::success ){
				printf( "<=== ERROR CODE ===> SynchroPort::read_data < %zd >\n" 
						, this->error_code);
			}
		return this->buffer_size;
	}


	size_t SynchroPort::write_data( std::vector< uint8_t >& buffer , size_t& size ){
		#ifdef _DEBUG_CONNECTION_
			printf( "want write size < %zd > ===" , size );
		#endif
			this->buffer_size = boost::asio::write( this->io_port
													, boost::asio::buffer( buffer , size ) 
													, this->error_code );
		#ifdef _DEBUG_CONNECTION_
			printf( "--- < %zd >\n" , this->buffer_size);
		#endif
			if( this->error_code != errc::success ){
				printf( "<=== ERROR CODE ===> SynchroPort::write_data < %zd >\n" 
						, this->error_code);
			}
	}

}
