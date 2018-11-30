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

//#define _ACK_OR_NACK_
//#define _DEBUG_SIZE_PACKET_
#define _DEBUG_DATA_STREAM_

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
		#ifdef _ACK_OR_NACK_
			this->print_buffer( "After adding sum IDLE ");
		#endif
		this->get_size_packet();
		this->write_data( this->buffer_packet , this->temp_size );	
		this->read_reply_packet( this->temp_boolean 
								, MIP_COMMUNICATION::COMMAND::BASE::DESCRIPTOR);
		#ifdef _ACK_OR_NACK_
			this->print_buffer( "Reply Packet after IDLE " );
		#endif
		if( this->buffer_packet[ this->buffer_packet.size() - 3 ] == 0x00 ){
			result = true;
		}
		else{
			result = false;
		}
	}

	void LordMicrostrain::command_resume( bool& result ){
		this->init_header_packet();
		this->add_data_to_packet( MIP_COMMUNICATION::COMMAND::BASE::DESCRIPTOR );
		this->add_data_to_packet( 0x02 );
		this->add_data_to_packet( 0x02 );
		this->add_data_to_packet( MIP_COMMUNICATION::COMMAND::BASE::RESUME );
		this->adding_check_sum();
		this->get_size_packet();
		this->write_data( this->buffer_packet , this->temp_size );
		this->read_reply_packet( this->temp_boolean 
								, MIP_COMMUNICATION::COMMAND::BASE::DESCRIPTOR );
		if( ! this->temp_boolean ) result = false ; return;
		if( *(this->buffer_packet_last - 3 ) == 0x00 ) result = true;
		else result = false;
	}

	void LordMicrostrain::sensor_get_IMU_base_rate( bool &result , int& base_rate ){
		this->init_header_packet();
		this->add_data_to_packet( MIP_COMMUNICATION::COMMAND::SENSOR::DESCRIPTOR );
		this->add_data_to_packet( 0x02 );
		this->add_data_to_packet( 0x02 );
		this->add_data_to_packet( MIP_COMMUNICATION::COMMAND::SENSOR::GET_IMU_DATA_BASE_RATE );
		this->adding_check_sum();
		#ifdef _ACK_OR_NACK_
			this->print_buffer( "After adding sum get data base rate ");
		#endif
		this->temp_size = ( size_t ) this->buffer_packet.size();
		this->write_data( this->buffer_packet , this->temp_size );
		this->read_reply_packet( this->temp_boolean 
								, MIP_COMMUNICATION::COMMAND::SENSOR::DESCRIPTOR );
		#ifdef _ACK_OR_NACK_
			this->print_buffer( "Reply Packet of cammnad get data base rate " );
		#endif
		if( *( this->buffer_packet_last - 7 ) == 0x00 ){
			base_rate = int ( 
						(uint16_t)(  *(this->buffer_packet_last - 4) << 8 )
						+ (uint16_t)( *(this->buffer_packet_last - 3 ) ) );
			result = true;
		}
		else{
			result = false;
		}
	}

	void LordMicrostrain::set_IMU_rate( int IMU_rate ){
		if( this->IMU_rate_packet.size() < 2 ){
			this->IMU_rate_packet.resize( 2 );
		}	
		this->IMU_rate_packet[ 0 ] = uint8_t(uint16_t( IMU_rate >> 8 & 0xff ) );
		this->IMU_rate_packet[ 1 ] = uint8_t(uint16_t( IMU_rate & 0xff ) );
	}

	void LordMicrostrain::sensor_init_setup_IMU_format( int type_message ){
		this->init_header_packet();
		this->add_data_to_packet( MIP_COMMUNICATION::COMMAND::SENSOR::DESCRIPTOR );
		this->add_data_to_packet( uint8_t(4 + ( 3 * type_message ) ) );
		this->add_data_to_packet( uint8_t(4 + ( 3 * type_message ) ) );
		this->add_data_to_packet( MIP_COMMUNICATION::COMMAND::SENSOR::IMU_MESSAGE_FORMAT );
		this->add_data_to_packet( 0x01 );
		this->add_data_to_packet( uint8_t( type_message ) );
	}

	void LordMicrostrain::sensor_add_message_type( uint8_t descriptor ){
		this->add_data_to_packet( descriptor );
		this->add_data_to_packet( this->IMU_rate_packet[0] );
		this->add_data_to_packet( this->IMU_rate_packet[1] );
	}

	void LordMicrostrain::sensor_setup_IMU_format( bool& result){
		this->adding_check_sum();
		this->get_size_packet();
		this->write_data( this->buffer_packet , this->temp_size );
		this->read_reply_packet( this->temp_boolean 
								, MIP_COMMUNICATION::COMMAND::SENSOR::DESCRIPTOR );
		#ifdef _ACK_OR_NACK_
			this->print_buffer( "Reply packet for set IMU message ");
		#endif
		if( *( this->buffer_packet_last - 3 ) == 0x00 ){
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
		#ifdef _ACK_OR_NACK_
			this->print_buffer( "After adding sum PING ");
		#endif
		this->get_size_packet();
		this->write_data( this->buffer_packet , this->temp_size);	
	}

	void LordMicrostrain::sensor_save_message_format( bool& result ){
		this->init_header_packet();
		this->add_data_to_packet( MIP_COMMUNICATION::COMMAND::SENSOR::DESCRIPTOR );
		this->add_data_to_packet( 0x08 );
		this->add_data_to_packet( 0x04 );
		this->add_data_to_packet( MIP_COMMUNICATION::COMMAND::SENSOR::IMU_MESSAGE_FORMAT );
		this->add_data_to_packet( 0x03 );
		this->add_data_to_packet( 0x00 );
		this->add_data_to_packet( 0x04 );
		this->add_data_to_packet( MIP_COMMUNICATION::COMMAND::SENSOR::ESTIMATION_FILTER_MESSAGE_FORMAT );
		this->add_data_to_packet( 0x03 );
		this->add_data_to_packet( 0x00 );
		this->adding_check_sum();
		#ifdef _ACK_OR_NACK_
			this->print_buffer( "After save setting message type ");
		#endif
		this->get_size_packet();
		this->write_data( this->buffer_packet , this->temp_size );
		this->read_reply_packet( this->temp_boolean 
								, MIP_COMMUNICATION::COMMAND::SENSOR::DESCRIPTOR );
		#ifdef _ACK_OR_NACK_
			this->print_buffer(" Reply packet for save setting IMU message ");
		#endif
		if( this->temp_boolean == false ) result = false ; return;
		if( *(this->buffer_packet_last - 3 ) == 0x00 && 
			*(this->buffer_packet_last - 7 ) == 0x00 ) result = true;
		else result = false;
	}

	void LordMicrostrain::sensor_enable_data_stream( bool use_IMU , bool use_estimation 
													, bool& result ){
		this->init_header_packet();
		this->add_data_to_packet( MIP_COMMUNICATION::COMMAND::SENSOR::DESCRIPTOR );
		this->add_data_to_packet( 0x0A );
		this->add_data_to_packet( 0x05 );
		this->add_data_to_packet( 0x11 );
		this->add_data_to_packet( 0x01 );
		this->add_data_to_packet( 0x01 );
		if( use_IMU ) this->add_data_to_packet( 0x01 );
		else this->add_data_to_packet( 0x00 );
		this->add_data_to_packet( 0x05 );
		this->add_data_to_packet( 0x11 );
		this->add_data_to_packet( 0x01 );
		this->add_data_to_packet( 0x01 );
		if( use_estimation ) this->add_data_to_packet( 0x01 );
		else this->add_data_to_packet( 0x00 );
		this->adding_check_sum();
		this->get_size_packet();
		this->write_data( this->buffer_packet , this->temp_size );
		this->read_reply_packet( this->temp_boolean 
								, MIP_COMMUNICATION::COMMAND::SENSOR::DESCRIPTOR );
		#ifdef _ACK_OR_NACK_
			this->print_buffer(" Reply packet for save setting IMU message ");
		#endif
		if( this->temp_boolean == false ) result = false ; return;
		if( *(this->buffer_packet_last - 3 ) == 0x00 && 
			*(this->buffer_packet_last - 7 ) == 0x00 ) result = true;
		else result = false;	
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
			printf("Warning < %s > require data size of packet\n" , this->name_port.c_str());
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

	void LordMicrostrain::get_size_packet(){
		this->temp_size = size_t( this->buffer_packet_last - this->buffer_packet_begin );
		#ifdef _DEBUG_SIZE_PACKET_
			printf( "size of packet is %zd \n" , this->temp_size );
		#endif
	}

	void LordMicrostrain::read_data_stream( std::vector< uint8_t > buffer_data_stream 
											, bool &result ){
		this->read_reply_packet( result , MIP_COMMUNICATION::DATA::IMU_DATA_SET::DESCRIPTOR );
		#ifdef _DEBUG_DATA_STREAM_
			printf( 
				"DETAILED output BUFFER : size -> %zd --- capacity -> %zd --- max_size -> %zd\n"
				, buffer_data_stream.size() 
				, buffer_data_stream.capacity() 
				, buffer_data_stream.max_size() );
		#endif
		if( ! result ) return;
		this->get_size_packet();
		if( buffer_data_stream.size() < this->temp_size ){
			buffer_data_stream.resize( this->temp_size );
		}
		int run = 0;
		for( this->buffer_packet_current = this->buffer_packet_begin 
			; this->buffer_packet_current < this->buffer_packet_last 
			; this->buffer_packet_current++ , run++ ){
			buffer_data_stream[run] = *( this->buffer_packet_current );
			#ifdef _DEBUG_DATA_STREAM_
				printf( "%2x " , buffer_data_stream[run]);
			#endif
		}
		
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

	void LordMicrostrain::find_check_sum( bool& result ){
		MSB = 0;
		LSB = 0;
		#ifdef _ACK_OR_NACK_
			this->print_buffer( "Reply buffer want to check");
		#endif
		for( this->buffer_packet_current = this->buffer_packet_begin 
				; this->buffer_packet_current < this->buffer_packet_last - 2  
				; this->buffer_packet_current++ ){
			MSB += *( this->buffer_packet_current );
			LSB += MSB;
		}
		#ifdef _ACK_OR_NACK_
			printf( "MSB :: LSB is %2X :: %2X\n", MSB , LSB );
		#endif
		if( MSB == *(this->buffer_packet_last - 2 )  &&
			LSB == *(this->buffer_packet_last - 1 ) ){
			result = true;
			printf("<------ IMU GOOD PACKET ------>\n");
		}
		else{
			result = false;
			printf("<------ IMU BAD PACKET ------>\n");
		}

	}
			
}
