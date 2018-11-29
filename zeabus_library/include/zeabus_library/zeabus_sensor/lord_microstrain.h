/*
	File name			:	lord_microstrain.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , NOV 27
	Date last modified	:	2018 , ??? ??
	Purpose				:	This is header of file about manage connect with IMU

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Production

	Namespace			:	zeabus_sensor
*/

#include	<zeabus_library/zeabus_sensor/synchronous_port.h>
#include	<zeabus_library/zeabus_sensor/MIP_COMMUNICATION.h>

#ifndef _ZEABUS_LIBRARY_ZEABUS_SENSOR_LORD_MICROSTRAIN_H__
#define _ZEABUS_LIBRARY_ZEABUS_SENSOR_LORD_MICROSTRAIN_H__

namespace zeabus_sensor{

	class LordMicrostrain : public SynchroPort {

		public:
			LordMicrostrain( std::string name_port );
			~LordMicrostrain();

			void command_idle( bool& result );
			void command_ping( bool& result );
			void sensor_get_IMU_base_rate( bool& result , int& base_rate );
			void set_IMU_rate( int IMU_rate );
			void sensor_init_setup_IMU_format( int type_message );
			void sensor_add_message_type( uint8_t descriptor );	
			void sensor_setup_IMU_format( bool& result );

			// this part is function to check about buffer ( data and memory used)
			void echo_detail_buffer();
			void print_buffer( std::string message = "" );

		protected:
			// this part use about setting or prepare packet for sending
			void init_header_packet();
			void add_data_to_packet( uint8_t data_byte );
			void adding_check_sum();
			void get_size_packet();

			// this part will use to check sum packet correct or not
			void find_check_sum( bool& result );
			void read_reply_packet( bool &result , uint8_t descriptor_set_byte
									, int limit_round = 2);
		
		private:
			uint8_t MSB;
			uint8_t LSB;
			size_t temp_size;
			size_t buffer_size;
			std::vector< uint8_t > IMU_rate_packet;
			std::vector< uint8_t > buffer_packet;
			std::vector< uint8_t > buffer_receive_bytes;
			std::vector< uint8_t >::iterator buffer_packet_begin;
			std::vector< uint8_t >::iterator buffer_packet_end;
			std::vector< uint8_t >::iterator buffer_packet_current;
			std::vector< uint8_t >::iterator buffer_packet_last;
	};
}

#endif
