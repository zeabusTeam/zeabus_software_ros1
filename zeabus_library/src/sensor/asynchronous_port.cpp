/*
	File name			:	asynchronous_port.cpp
	Author				:	Supasan Komonlit
	Date created		:	2018 , NOV 28
	Date last modified	:	2018 , DEC 26
	Purpose				:	This is source of file about port style asynchronous

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.1.0
	status				:	STOP
							
	Namespace			:	zeabus_library/sensor
*/

#include	<zeabus_library/sensor/serial_port.h>

#include	<vector> // use to make buffer for get data

#include	<stdio.h> // standard library of c language
#include	<iostream> // standard library of cpp language

namespace zeabus_library{

namespace sensor{

	Asynchronous::AynchroPort( std::string name_port ) :
		SerialPort( name_port ){};

	Asynchronous::~AynchroPort(){};

	void Asynchronous::read_handle( const boost::system::error_code& error_code
							, const size_t bytes_transfereed ){

	}

	void Asynchronous::write_handle( const boost::system::error_code& error_code
							, const size_t bytes_transfereed ){

	}

	void Asynchronous::time_handle( const boost::system::error_code& error_code){
		if( error_code != errc::success && ( this->io_state == IO_PROCESS ) ){
			this->io_state = IO_TIME_OUT;
		}
	}

	size_t Asynchronous::read_data( std::vector<uint8_t>& buffer , size_t& size , int time_out){

		if( buffer.size() < size ){
			buffer.resize( size ); // for make real buffer must ensure have size to collect data
		}

		
	}

	size_t Asynchronous::write_data( std::vector<uint8_t>& buffer , size_t& size , int time_out){

	}

}
}
