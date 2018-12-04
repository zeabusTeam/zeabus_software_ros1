/*
	File name			:	asynchronous_port.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , NOV 28
	Date last modified	:	2018 , ??? ??
	Purpose				:	This is header of file abount port style asynchronous

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.0.1
	status				:	Prouction
							
	Namespace			:	zeabus_sensor
*/

#include	<zeabus_library/zeabus_sensor/serial_port.h>

#include	<boost/chrono.hpp> // have to use time for check time out
#include	<boost/bind.hpp> // have to use bind for send function in class to callback

#include	<vector> // use to make buffer for get data

#include	<stdio.h> // standard library of c language
#include	<iostream> // standard library of cpp language

#ifndef _ZEABUS_LIBRARY_ZEABUS_SENSOR_SYNCHRONOUS_PORT_H__
#define _ZEABUS_LIBRARY_ZEABUS_SENSOR_SYNCHRONOUS_PORT_H__

namespace zeabus_sensor{

	class AsynchroPort : public SerialPort{

		public:
	
			AynchroPort( std::string name_port ); // this function will cal when init object
			~AynchroPort(); // this function will call when delete object

			void read_handle( const boost::system::error_code& error_code
							, const size_t bytes_transfereed );

			void write_handle( const boost::system::error_code& error_code
							, const size_t bytes_transfereed );

			void time_handle( const boost::system::error_code& error_code);

			size_t read_data( std::vector<uint8_t>& buffer , size_t& size , int time_out );
			size_t write_data( std::vector<uint8_t>& buffer , size_t& size , int time_out );

		private:
			uint8_t io_state;
			boost::asio::deadline_timer timer;
			size_t buffer_size;

	};

}

#endif 
