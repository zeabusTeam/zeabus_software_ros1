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
			std::cout << "write set_idle command : ";
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
		unsigned int microstrain_imu_port::get_imu_data_base_rate(){
			this->adding_header( (this->write_buffer ) );
			this->write_buffer.push_back( COMMAND::SENSOR::DESCRIPTOR );
			this->write_buffer.push_back( 0x02 );
			this->write_buffer.push_back( 0x02 );
			this->write_buffer.push_back( COMMAND::SENSOR::GET_IMU_DATA_BASE_RATE );
			this->adding_checksum( this->write_buffer);
			this->print_vector( this->write_buffer , "data write get imu base rate ");
			do{
				this->write_asynchronous( this->write_buffer , this->write_buffer.size());
				this->read_reply_command("get imu base rate ");
			}while( this->check_ACK_NACK( this->read_buffer.size() -7));

			uint16_t answer = (uint16_t)(this->read_buffer[ this->read_buffer.size() - 4] << 8)
						+ (uint16_t)(this->read_buffer[ this->read_buffer.size()-3]);
			std::cout << "answer is " << std::hex << answer << std::dec << "\n";
			return int( answer );
		}

		void microstrain_imu_port::set_imu_message_format_field( uint16_t decimation
				, std::vector<uint8_t> descriptor_data){
			// assign lengt in this field
			uint8_t field_length = 4 + 3*descriptor_data.size();	
			uint8_t field_descriptor = COMMAND::SENSOR::IMU_MESSAGE_FORMAT;
			uint8_t function = 0x01; // use new setting
			this->imu_message_format_field.resize(0); // new std::vector for correct field
			this->imu_message_format_field.push_back( field_length );
			this->imu_message_format_field.push_back( field_descriptor);
			this->imu_message_format_field.push_back( function );
			for( int run = 0 ; run < descriptor_data.size() ; run++ ){
				this->imu_message_format_field.push_back( descriptor_data[run]);
				this->imu_message_format_field.push_back( ( uint8_t(decimation >> 8) & 0xff ));
				this->imu_message_format_field.push_back( uint8_t ( decimation & 0xff));
			}
			#ifdef TEST_IMU_PORT
				this->print_vector( this->imu_message_format_field , "FIELD IMU FORMAT ");
			#endif
		}

		void microstrain_imu_port::set_imu_message_format(){
			this->adding_header( this->write_buffer);
			this->write_buffer.push_back( COMMAND::SENSOR::DESCRIPTOR);
			this->write_buffer.push_back( this->imu_message_format_field[0]);
			for( int run = 0 ; run < this->imu_message_format_field.size() ; run++){
				this->write_buffer.push_back( this->imu_message_format_field[run]);
			}
			this->adding_checksum( this->write_buffer);
			this->print_vector( this->write_buffer , "data write IMU MESSAGE FORMAT ");
			do(
				this->write_asynchronous( this->write_buffer , this->write_buffer.size());
				this->read_reply_command("reply of set imu message format ");
			)while( this->check_ACK_NACK( this->read_buffer.size() - 3));
		}

		
	}

}
