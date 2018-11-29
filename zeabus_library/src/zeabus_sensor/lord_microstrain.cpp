/*
	File name			:	lord_microstrain.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2018 , NOV 27
	Date last modified	:	2018 , ??? ??
	Purpose				:	This is source of file about connection with port imu

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Production

	Namespace			:	zeabus_sensor
*/

#include <zeabus_library/zeabus_sensor/lord_microstrain.h>

#define ACK_OR_NACK

namespace zeabus_sensor{
	
	LordMicrostrain::LordMicrostrain( std::string name_port ) : SynchroPort( name_port ){
		this->buffer_packet.resize( 100 );
		this->buffer_packet.reserve( 100 );
		this->buffer_packet_begin = this->buffer_packet.begin();
		this->buffer_packet_end = this->buffer_packet.end(); 
	}

	LordMicrostrain::~LordMicrostrain(){}

	void LordMicrostrain::command_idle( bool& result ){
		this->init_header_packet();
		this->add_data_to_packet( MIP_COMMUNICATION::COMMAND::BASE::DESCRIPTOR );
		this->add_data_to_packet( 0x02 );
		this->add_data_to_packet( 0x02);
		this->add_data_to_packet( MIP_COMMUNICATION::COMMAND::BASE::IDLE );
		this->adding_check_sum();
		this->print_buffer( "After adding sum IDLE ");
		this->echo_detail_buffer();
		this->temp_size = (size_t) this->buffer_packet.size();
		this->write_data( this->buffer_packet , this->temp_size );	
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

	void LordMicrostrain::sensor_get_IMU_base_rate( bool &result , int& base_rate ){
		this->init_header_packet();
		this->add_data_to_packet( MIP_COMMUNICATION::COMMAND::SENSOR::DESCRIPTOR );
		this->add_data_to_packet( 0x02 );
		this->add_data_to_packet( 0x02 );
		this->add_data_to_packet( MIP_COMMUNICATION::COMMAND::SENSOR::GET_IMU_DATA_BASE_RATE );
		this->adding_check_sum();
		this->print_buffer( "After adding sum get data base rate ");
		this->echo_detail_buffer();
		this->temp_size = ( size_t ) this->buffer_packet.size();
		this->write_data( this->buffer_packet , this->temp_size );
		this->read_reply_packet( this->temp_boolean 
								, MIP_COMMUNICATION::COMMAND::SENSOR::DESCRIPTOR );
		this->print_buffer( "Reply Packet of cammnad get data base rate " );
		if( this->buffer_packet[ this->buffer_packet.size() - 7 ] == 0x00 ){
			base_rate = int ( 
						(uint16_t)(  *(this->buffer_packet_last - 4) << 8 )
						+ (uint16_t)( *(this->buffer_packet_last - 3 ) ) );
			result = true;
		}
		else{
			result = false;
		}
	}

	void LordMicrostrain::command_ping( bool& result ){
		this->init_header_packet();
		this->add_data_to_packet( MIP_COMMUNICATION::COMMAND::BASE::DESCRIPTOR );
		this->add_data_to_packet( 0x02 );
		this->add_data_to_packet( MIP_COMMUNICATION::COMMAND::BASE::PING );
		this->adding_check_sum();
		this->print_buffer( "After adding sum PING ");
		this->echo_detail_buffer();
		this->temp_size = ( size_t ) this->buffer_packet.size(); 
		this->write_data( this->buffer_packet , this->temp_size);	
	}	
			
	void LordMicrostrain::echo_detail_buffer(){
		printf( "DETAILED MEMORY BUFFER : size -> %zd --- capacity -> %zd --- max_size -> %zd\n",
					this->buffer_packet.size() , this->buffer_packet.capacity() 
					, this->buffer_packet.max_size() );
		printf( "DETAILED MEMORY TEMP : size -> %zd --- capacity -> %zd --- max_size -> %zd\n",
					this->buffer_packet.size() , this->buffer_packet.capacity() 
					, this->buffer_packet.max_size() );
	}
			

	void LordMicrostrain::print_buffer( std::string message ){
		printf( "%s Data packet is : " , message.c_str() );
		for( this->buffer_packet_current = this->buffer_packet_begin 
				; this->buffer_packet_current < this->buffer_packet_last 
				; this->buffer_packet_current++ ){
			printf( "%2X " , *( this->buffer_packet_current) );
		}
		printf("\n");
	}

	void LordMicrostrain::init_header_packet(){
		this->buffer_packet_current = this->buffer_packet_begin;
		this->add_data_to_packet( 0x75 );
		this->add_data_to_packet( 0x65 );
	}

	void LordMicrostrain::add_data_to_packet( uint8_t data_byte ){
		if( this->buffer_packet_current == this->buffer_packet_end ){
			printf("Warning < %s > require data size of packer\n" , this->name_port.c_str());
			this->buffer_packet.push_back( data_byte );
			this->buffer_packet_end = this->buffer_packet.end();
			this->buffer_packet_current += 1;
		}
		else{
			*(this->buffer_packet_current) = data_byte;
			this->buffer_packet_current += 1;
		}
		this->buffer_packet_last = this->buffer_packet_current;
	}

	void LordMicrostrain::adding_check_sum(){
		MSB = 0;
		LSB = 0;
		for( this->buffer_packet_current = this->buffer_packet_begin 
				; this->buffer_packet_current != this->buffer_packet_last 
				; this->buffer_packet_current++ ){
			MSB += *( this->buffer_packet_current ) ;
			LSB += MSB;
		}
		this->add_data_to_packet( MSB );
		this->add_data_to_packet( LSB );
	}

	void LordMicrostrain::read_reply_packet( bool &result, uint8_t descriptor_set_byte
											, int limit_round){
		result = false;

		int count = 0;

		while( ! temp_boolean && count < limit_round){
			count ++ ;
			this->temp_size = 1;
			if( 1 == this->read_data( this->buffer_receive_bytes , this->temp_size ) ) {
				if( this->buffer_receive_bytes[0] != 'u' ) continue;
			}
			else continue;
			
			if( 1 == this->read_data( this->buffer_receive_bytes , this->temp_size ) ){
				if( this->buffer_receive_bytes[0] != 'e' ) continue;
			}
			else continue;
		
			this->init_header_packet();
			
			if( 1 == this->read_data( this->buffer_receive_bytes , this->temp_size ) ){
				if( this->buffer_receive_bytes[0] != descriptor_set_byte ){
					printf( "Wrong packet descriptor set byte \n");
					continue;
				}
				else{
					this->add_data_to_packet( this->buffer_receive_bytes[0] );
				}
			}
			else continue;

			if( 1 == this->read_data( this->buffer_receive_bytes , this->temp_size ) ) {
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

			this->temp_size = 2;
			if( 2 == this->read_data( this->buffer_receive_bytes , this->temp_size ) ){
				this->add_data_to_packet( this->buffer_receive_bytes[0] );
				this->add_data_to_packet( this->buffer_receive_bytes[1] );
			}
			else continue;

			this->find_check_sum( result );
		}
				
	}

	void LordMicrostrain::find_check_sum( bool& result){
		MSB = 0;
		LSB = 0;
		this->print_buffer( "Reply buffer want to check");
		for( this->buffer_packet_current = this->buffer_packet_begin 
				; this->buffer_packet_current < this->buffer_packet_last - 2  
				; this->buffer_packet_current++ ){
			MSB += *( this->buffer_packet_current );
			LSB += MSB;
		}
		printf( "MSB :: LSB is %2X :: %2X\n", MSB , LSB );
		if( MSB == *(this->buffer_packet_last - 2 )  &&
			LSB == *(this->buffer_packet_last - 1 ) ){
			result = true;
			printf("<------ GOOD_REPLY ------>\n");
		}
		else{
			result = false;
			printf("<------ BAD_REPLY ------>\n");
		}

	}
			
}
