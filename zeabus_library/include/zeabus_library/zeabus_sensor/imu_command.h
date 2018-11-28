/*
	File name			:	imu_command.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , NOV 27
	Date last modified	:	2018 , ??? ??
	Purpose				:	This is header of file about manage connect with IMU

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.0.5
	status				:	Production

	Namespace			:	zeabus_sensor
*/

#include	<zeabus_library/zeabus_sensor/synchronous_port.h>
#include	<zeabus_library/zeabus_sensor/MIP_COMMUNICATION.h>

#ifndef _ZEABUS_LIBRARY_ZEABUS_SENSOR_IMU_PORT__
#define _ZEABUS_LIBRARY_ZEABUS_SENSOR_IMU_PORT__

namespace zeabus_sensor{

	class IMUPort : public SynchronousPort {

		public:
			IMUPort( std::string name_port );
			~IMUPort();

			void command_idle( bool& result );
			void command_ping( bool& result );	

			// this part is function to check about buffer ( data and memory used)
			void echo_detail_buffer();
			void print_buffer( std::string message = "" );

		protected:
			// this part use about setting or prepare packet for sending
			void init_header_packet();
			void add_data_to_packet( uint8_t data_byte );
			void adding_check_sum();

			// this part will use to check sum packet correct or not
			void find_check_sum( bool& result );
			void read_reply_packet( bool &result , uint8_t descriptor_set_byte
									, int limit_round = 10);
		
		private:
			uint8_t MSB;
			uint8_t LSB;
			size_t buffer_size;
			std::vector< uint8_t > buffer_packet;
			std::vector< uint8_t > buffer_receive_bytes;
	};
}

#endif
