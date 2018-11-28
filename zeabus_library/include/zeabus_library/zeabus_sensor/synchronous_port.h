/*
	File name			:	synchronous_port.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , NOV 28
	Date last modified	:	2018 , NOV 28
	Purpose				:	This is header of file abount port style synchronous

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.0.1
	status				:	Stop Maintain
							< I think when you want read data from IMU  must use Asynchronous>
	Namespace			:	zeabus_sensor
*/

#include	<zeabus_library/zeabus_sensor/serial_port.h>

#include	<vector> // use to make buffer for get data

#include	<stdio.h> // standard library of c language
#include	<iostream> // standard library of cpp language

#ifndef _ZEABUS_LIBRARY_ZEABUS_SENSOR_SYNCHRONOUS_PORT_H__
#define _ZEABUS_LIBRARY_ZEABUS_SENSOR_SYNCHRONOUS_PORT_H__

namespace zeabus_sensor{

	class SynchroPort : public SerialPort{

		public:
	
			SynchroPort( std::string name_port ); // this function will cal when init object
			~SynchroPort(); // this function will call when delete object

			size_t read_data( std::vector<uint8_t>& buffer , size_t& size );
			size_t write_data( std::vector<uint8_t>& buffer , size_t& size );

		private:
			size_t buffer_size;

	};

}

#endif 
