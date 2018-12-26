/*
	File name			:	serial_port.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , NOV 26
	Date last modified	:	2018 , DEC 27
	Purpose				:	This is header of file about standard doing to do with port

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.1.0
	status				:	Production

	Namespace			:	zeabus_sensor
*/

#include	<boost/asio.hpp> // include library boost about serial port

#include	<vector> // use to make buffer for get data

#include	<stdio.h> // standard library of c language
#include	<iostream> // standard library of cpp language

/*	SENSOR OPTIONS DOCUMENTS 
		baud rate		: rate for send symbol of message. Ever learn in DATA COMMUNICATION
		flow control	: The ability to stop and then restart flow without any loss of bytes
		parity			: method of detecting error in transmission
		stop bits		: stop bits sent at the end of every character	
		character_size	: size bits of character
	Ref :	https://www.tldp.org/HOWTO/Serial-HOWTO-4.html
			https://en.wikipedia.org/wiki/Serial_port#Parity
	Looking about error code at
		:	https://www.boost.org/doc/libs/1_61_0/boost/system/error_code.hpp
*/

#ifndef _ZEABUS_LIBRARY_SENSOR_SERIAL_PORT_H__
#define _ZEABUS_LIBRARY_SENSOR_SERIAL_PORT_H__

namespace errc = boost::system::errc;

namespace zeabus_library{

namespace sensor{

	const uint8_t IO_ERROR = 0;
	const uint8_t IO_DONE = 1;
	const uint8_t IO_PROCESS = 2;
	const uint8_t IO_TIME_OUT = 3;

	class SerialPort : private boost::noncopyable{

		public:	
			~SerialPort(); // this function will call when delete object

			void open_port( bool& result );

			void is_open( bool& result );

			void close_port( bool& result );
			
			void set_option_port( boost::asio::serial_port_base::baud_rate baud_rate);
			void set_option_port( boost::asio::serial_port_base::flow_control flow_control);
			void set_option_port( boost::asio::serial_port_base::parity parity);
			void set_option_port( boost::asio::serial_port_base::stop_bits stop_bits);
			void set_option_port( boost::asio::serial_port_base::character_size character_size);

			void get_option_port( boost::asio::serial_port_base::baud_rate& baud_rate);
			void get_option_port( boost::asio::serial_port_base::flow_control& flow_control);
			void get_option_port( boost::asio::serial_port_base::parity& parity);
			void get_option_port( boost::asio::serial_port_base::stop_bits& stop_bits);
			void get_option_port( boost::asio::serial_port_base::character_size& character_size);
		
		protected:
			SerialPort( std::string name_port ); // this function will cal when init object
			boost::asio::io_service io_service;
			boost::asio::serial_port io_port;
			boost::system::error_code error_code;
			bool temp_boolean;
			std::string name_port;
				
		
	};
}
}

#endif
