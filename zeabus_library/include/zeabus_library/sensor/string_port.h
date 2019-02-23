/*
	File name			:	string_port.h		
	Author				:	Supasan Komonlit
	Date created		:	2019 , JAN 12
	Date last modified	:	2019 , ??? ??
	Purpose				:	This is header of file abount port style synchronous and using string

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Production

	Namespace			:	zeabus_library/sensor
*/

#include	<zeabus_library/sensor/serial_port.h>

#include	<vector> // use to make buffer for get data

#include	<stdio.h> // standard library of c language
#include	<iostream> // standard library of cpp language

#ifndef _ZEABUS_LIBRARY_SENSOR_STRING_PORT_H__
#define _ZEABUS_LIBRARY_SENSOR_STRING_PORT_H__

namespace zeabus_library{

namespace sensor{

	class StringPort : public SerialPort{

		public:
	
			StringPort( std::string name_port ); // this function will cal when init object
			~StringPort(); // this function will call when delete object

			size_t read_data( std::string& message );
			size_t write_data( std::string& message );
			size_t write_data( std::string message );

		private:
			char temp;
			size_t buffer_size;
			size_t count;
	};

}
}

#endif 
