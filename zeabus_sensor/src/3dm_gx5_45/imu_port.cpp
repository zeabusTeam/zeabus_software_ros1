/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File	: imu_port.cpp
//
//	Create	: Oct 09 , 2018
//	Author	: Supasan Komonlit
//
//	Main Purpose : Detail of cord from imu_port
//				
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ZEABUS_SENSOR_IMU_PORT_H
	#include	<zeabus_sensor/imu_port.h>
	#define ZEABUS_SENSOR_IMU_PORT_H
#endif
//#include	"imu_port.h"

#define TEST_IMU_PORT

namespace zeabus_sensor{

	namespace MIP_COMMUNICATION{

		microstrain_imu_port::microstrain_imu_port( std::string name_port ): 
				zeabus_extension::manage_port::specific_port( name_port )
		{ 
			#ifdef TEST_IMU_PORT
				std::cout	<< "<--TESTER--> Init call microstrain_imu_port by name port is "
							<< name_port << "\n";
			#endif
		} 

		microstrain_imu_port::~microstrain_imu_port(){
			#ifdef TEST_IMU_PORT
				std::cout	<< "<--TESTER--> THE END OF microstrain_imu_port\n";
			#endif
		}		

		void microstrain_imu_port::set_idle(){
			this->adding_header( this->write_buffer );
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
			this->write_asynchronous( this->write_buffer , this->write_buffer.size());
			this->read_reply_command("Set to Idle");
		}

		template<typename type_vector>void microstrain_imu_port::adding_header(
												type_vector data){
			data.resize(0);
			//data.push_back( this->sync_1);
			//data.push_back( this->sync_2);
			data.push_back( 'u');
			data.push_back( 'e');
			#ifdef TEST_IMU_PORT
				std::cout	<< "<--TESTER--> After adding_head have size " 
							<< data.size() << "\n";
			#endif
		}

		template<typename type_vector>void microstrain_imu_port::adding_checksum( 
												type_vector data){
			uint16_t check_sum_01 = 0;
			uint16_t check_sum_02 = 0;
			for( int run = 0 ; run < data.size() ; run++){
				check_sum_01 += data[run];
				check_sum_02 += check_sum_01;
				#ifdef TEST_IMU_PORT
					printf("<---HOW---> %x : %x : %x" , data[run] , check_sum_01 , check_sum_02);
				#endif
			}
			
			uint32_t all_result = (check_sum_01 << 8) + check_sum_02;
			data.push_back( (uint8_t)((all_result>>8)&0xff));
			data.push_back( (uint8_t)(all_result&0xff));
			#ifdef TEST_IMU_PORT
				printf("<--RESULT--> %x : %x\n", *(data.end()-2) , *(data.end()-1));
				std::cout	<< "<--TESTER--> After adding_checksum have size is "
							<< data.size() << "\n"; 
			#endif
		}

		void microstrain_imu_port::read_reply_command( std::string command){
			// First read untill find 'u' or 0x75 to show start packet
			std::vector<unsigned uint8_t> temporary;
			while( true ){
				temporary = this->read_asynchronous( (size_t)1);
				if( temporary[0] == 'u' ){
					this->read_buffer.push_back( temporary[0] );
					break;
				}
			}

			// Second read untill find 'e' or 0x65 to show second start packet
			while( true ){
				temporary = this->read_asynchronous( (size_t)1 );
				//if( temporary[0] == this->sync_2){
				if( temporary[0] == 'e'){
					this->read_buffer.push_back( temporary[0] );
					break;
				}
			}

			// Third read 2 bytes for get Describe set [0] and Payload Length[1]
			temporary = this->read_asynchronous( (size_t)2);
			for( int run = 0 ; run < temporary.size() ; run++){
				this->read_buffer.push_back( temporary[run] );
			}
			
			// Read all data in reply fields
			temporary = this->read_asynchronous( (size_t)temporary[1]);
			for( int run = 0 ; run < temporary.size() ; run++){
				this->read_buffer.push_back( temporary[run] );
			}

			// Read about checksum and calculate for prove collect data
			temporary = this->read_asynchronous( (size_t)2);
			this->adding_checksum( this->read_buffer );
			bool ok_data = true;
			if( *(this->read_buffer.end()-2) == temporary[0]){
				#ifdef TEST_IMU_PORT
					std::cout << "<--TESTER--> MSB ARE EQUAL\n";
				#endif
			}
			else ok_data = false;
			if( *(this->read_buffer.end()-1) == temporary[1]){
				#ifdef TEST_IMU_PORT
					std::cout << "<--TESTER--> LSB ARE EQUAL\n";
				#endif
			}
			else ok_data = false;
				
			// Print all reply of command
			std::cout <<"<--IMU-->Result of " << command << " is ";
			for(	std::vector<uint8_t>::iterator iterator = this->read_buffer.begin() ;  
					iterator != this->read_buffer.end() ; iterator++
				){
				printf("%x" , *iterator);	
			}
			printf("\n");
					
		} 

}

}
