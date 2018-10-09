/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File	: imu_port.h
//
//	Create	: Oct 09 , 2018
//	Author	: Supasan Komonlit
//
//	Main Purpose : This file is for class imu for connection to imu port
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ZEABUS_EXTENSION_MANAGE_PORT_H
	#include	<zeabus_extension/manage_port.h>
	#define ZEABUS_EXTENSION_MANAGE_PORT_H
#endif

#ifndef IOSTREAM
	#include	<iostream>
	#define IOSTREAM
#endif

#ifndef ZEABUS_SENSOR_MIP_COMMUNICATION_H
	#include	<zeabus_sensor/MIP_COMMUNICATION_H>
	#define ZEABUS_SENSOR_MIP_COMMUNICATION_H
#endif

namespace zeabus_sensor{

	namespace MIP_COMMUNICATION{

		class microstrain_imu_port: public zeabus_extension::manage_port::specific_port{
			public:
				microstrain_imu_port( std::string name_port="");
				~microstrain_imu_port();
				void set_idle(); // function to set imu ready for config recommand
				unsigned int get_imu_data_base_rate(); // function for get IMU aFata BASE RATE

			private:
				static const unsigned char sync_1 = 0x75;
				static const unsigned char sync_2 = 0x65;
				template<typename type_vector>adding_header( type_vector data);
				template<typename type_vector>adding_checksum( type_vector data);
				std::vector<unsigned uint8_t> read_buffer;
				std::vector<unsigned uint8_t> write_buffer;
		}
	}

}
