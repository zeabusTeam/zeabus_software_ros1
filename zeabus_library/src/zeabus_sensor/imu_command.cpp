/*
	File name			:	imu_command.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2018 , NOV 27
	Date last modified	:	2018 , ??? ??
	Purpose				:	This is source of file about connection with port imu

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.0.5
	status				:	Production

	Namespace			:	zeabus_sensor
*/

#include <zeabus_library/zeabus_sensor/imu_command.h>

#define ACK_OR_NACK

namespace zeabus_sensor{
	
	IMUPort::IMUPort( std::string name_port ) : SynchronousPort( name_port ){}

	IMUPort::~IMUPort(){}

	void IMUPort::command_idle( bool& result ){
		this->init_header_packet();
		this->add_data_to_packet( MIP_COMMUNICATION::COMMAND::BASE::DESCRIPTOR );
		this->add_data_to_packet( 0x02 );
		this->add_data_to_packet( MIP_COMMUNICATION::COMMAND::BASE::IDLE );
		this->adding_check_sum();
		this->print_buffer( "After adding sum IDLE ");
		this->echo_detail_buffer();
		this->write_data( this->buffer_packet , size_t(this->buffer_packet.size()) );	
		this->read_reply_packet( this->temp_boolean 
								, MIP_COMMUNICATION::COMMAND::BASE::DESCRIPTOR);
		this->print_buffer( "Reply Packet after IDLE " );
		if( this->buffer_packet[ this->buffer_packet.size() - 3 ] == 0x00 ){
			result = true;
		}
		else{
			result = false;
		}
	}

	void IMUPort::command_ping( bool& result ){
		this->init_header_packet();
		this->add_data_to_packet( MIP_COMMUNICATION::COMMAND::BASE::DESCRIPTOR );
		this->add_data_to_packet( 0x02 );
		this->add_data_to_packet( MIP_COMMUNICATION::COMMAND::BASE::PING );
		this->adding_check_sum();
		this->print_buffer( "After adding sum PING ");
		this->echo_detail_buffer();
		this->write_data( this->buffer_packet , this->buffer_packet.size());	
	}	
			
	void IMUPort::echo_detail_buffer(){
		printf( "DETAILED MEMORY BUFFER : size -> %ld --- capacity -> %ld --- max_size -> %ld\n",
					this->buffer_packet.size() , this->buffer_packet.capacity() 
					, this->buffer_packet.max_size() );
	}
			

	void IMUPort::print_buffer( std::string message ){
		printf( "%sData packet is : " , message.c_str() );
		for( int run  = 0 ; run < this->buffer_packet.size() ; run++ ){
			printf( "%X " , this->buffer_packet[ run ] );
		}
		printf("\n");
	}

	void IMUPort::init_header_packet(){
		this->buffer_packet.clear();
		this->add_data_to_packet( 0x75 );
		this->add_data_to_packet( 0x65 );
	}

	void IMUPort::add_data_to_packet( uint8_t data_byte ){
		this->buffer_packet.push_back( data_byte );
	}

	void IMUPort::adding_check_sum(){
		MSB = 0;
		LSB = 0;
		for( int run = 0 ; run < this->buffer_packet.size() ; run++ ){
			MSB += this->buffer_packet[ run ];
			LSB += MSB;
		}
		this->add_data_to_packet( MSB );
		this->add_data_to_packet( LSB );
	}

	void IMUPort::read_reply_packet( bool &result , uint8_t descriptor_set_byte){

		result = false;

		while( ! temp_boolean ){

			if( 1 == this->read_data( this->buffer_receive_bytes , 1 ) ){
				if( this->buffer_receive_bytes[0] != 'u' ) continue;
			}
			else{
				printf("Can't read message function read_reply_packet\n");
				continue;
			}
			
			if( 1 == this->read_data( this->buffer_receive_bytes , 1 ) ){
				if( this->buffer_receive_bytes[0] != 'e' ) continue;
			}
			else continue;
		
			this->init_header_packet();
			
			if( 1 == this->read_data( this->buffer_receive_bytes , 1 ) ){
				if( this->buffer_receive_bytes[0] != descriptor_set_byte ){
					printf( "Wrong packet descriptor set byte \n");
					continue;
				}
				else{
					this->add_data_to_packet( this->buffer_receive_bytes[0] );
				}
			}
			else continue;

			if( 1 == this->read_data( this->buffer_receive_bytes , 1 ) ) {
				this->buffer_size = this->buffer_receive_bytes[0];
				this->add_data_to_packet( this->buffer_receive_bytes[0] );
			}
			else continue;

			if( this->buffer_size == this->read_data( this->buffer_receive_bytes 
													, this->buffer_size) ){
				for( int run = 0 ; run < this->buffer_size ; run++ ){
					this->add_data_to_packet( this->buffer_receive_bytes[ run ] );
				}
			}
			else continue;

			if( 2 == this->read_data( this->buffer_receive_bytes , 2 ) ){
				this->add_data_to_packet( this->buffer_receive_bytes[0] );
				this->add_data_to_packet( this->buffer_receive_bytes[1] );
			}
			else continue;

			this->find_check_sum( result );

		}
				
	}

	void IMUPort::find_check_sum( bool& result){
		MSB = 0;
		LSB = 0;
		for( int run = 0 ; run < this->buffer_packet.size() ; run++ ){
			MSB += this->buffer_packet[ run ];
			LSB += MSB;
		}
		if( MSB == this->buffer_packet[ this->buffer_packet.size() - 2 ] &&
			LSB == this->buffer_packet[ this->buffer_packet.size() - 1 ] ){
			result = true;
			printf("<------ GOOD_REPLY ------>\n");
		}
		else{
			result = false;
			printf("<------ BAD_REPLY ------>\n");
		}

	}
			
}
