/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File	: imu_command.cpp
//
//	Create	: Oct 09 , 2018
//	Author	: Supasan Komonlit
//
//	Main Purpose : Detail of specific command form imu_port 
//				
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ZEABUS_SENSOR_IMU_PORT_H
	#include	<zeabus_sensor/imu_port.h>
	#define ZEABUS_SENSOR_IMU_PORT_H
#endif

namespace zeabus_sensor{

	namespace MIP_COMMUNICATION{

		// set idle is recommend in first step to prepare config imu
		// it make imu already to config or reply command
		void microstrain_imu_port::set_idle(){
			this->adding_header( (this->write_buffer) );
			#ifdef TEST_IMU_PORT
				std::cout	<< "<--TESTER--> After adding_head in call function area have size "
							<< this->write_buffer.size() << "\n";
			#endif
			this->write_buffer.push_back( COMMAND::BASE::DESCRIPTOR ); // add descriptor set
			this->write_buffer.push_back( 0x02 ); // add Payload Length
			this->write_buffer.push_back( 0x02 ); // add Field Length
			this->write_buffer.push_back( COMMAND::BASE::IDLE ); // add descriptor field
			#ifdef TEST_IMU_PORT
				std::cout	<< "<--TESTER--> After adding Command field have size " 
							<< write_buffer.size() << "\n";
			#endif
			this->adding_checksum( this->write_buffer);
			#ifdef TEST_IMU_PORT
				std::cout	<< "<--TESTER--> After adding Check some have size " 
							<< write_buffer.size() << "\n";
			#endif
			std::cout << "write set_idle command ";
			for( int run = 0 ; run < this->write_buffer.size() ; run ++){
				printf("%x " , this->write_buffer[run]);
			}
			std::cout << "\n";
			do{ 
				this->write_asynchronous( this->write_buffer , this->write_buffer.size());
				this->read_reply_command("Set to Idle");
			}while( this->check_ACK_NACK( this->read_buffer.size() - 3));
		}

		// get base rate for the IMU data in Hz
		// this value is used for data rate calculations
		void microstrain_imu_port::get_imu_data_base_rate(){

		}
	}

}
