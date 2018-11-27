/*
	File name			:	imu_command.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , NOV 26
	Date last modified	:	2018 , ??? ??
	Purpose				:	This is header of file abount port style synchronous

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.0.1
	status				:	Production

	Namespace			:	zeabus_sensor
*/

#include	<zeabus_library/zeabus_sensor/synchronous_port.h>


namespace zeabus_sensor{

	class IMUPort : public SynchronousPort {

		public:
			IMUPort( std::string name_port );
			~IMUPort();

			command_idle( bool& result );
			command_ping( bool& result );	
			

		protected:
			void init_header_packet();
			void set_descriptor( uint8_t descriptor_set );
			void set_payload_length( uint8_t payload_length );

			void adding_check_sum();
			void find_check_sum( size_t length );
			
		private:
			std::vector< uint8_t > buffer_packet;

	};
}
