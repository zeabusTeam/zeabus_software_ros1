/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File	: serial.cpp
//
//	Edit	: Aug 15 , 2018
//	Author	: Supasan Komonlit
//
//	Thanks	: Mahisorn Nov 17 , 2014
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include	<zeabus_serial/serial.h>


//-------------------------------> the part of timeout error <---------------------------------//

zeabus_serial::timeout_error::timeout_error( bool is_write , unsigned int time) 
		: std::runtime_error( zeabus_serial::timeout_error::generate_string( is_write , time)){}

std::string zeabus_serial::timeout_error::generate_string( bool is_write , unsigned int time){
	std::stringstream temporary;
	temporary << "Timed out while " << ( (is_write) ? "writeing" : "reading" ) << "\t,\t";
	temporary << "Time out limit is " << time << "ms."
	return ss.str();
}

//---------------------------------> the part of IO error <------------------------------------//

zeabus_serial::IO_error::IO_error( const std::string& description )
		: std::runtime_error( description ){}

//------------------------------------> the part packet <--------------------------------------//

zeabus_serial::packet::packet(){}

virtual zeabus_serial::packet::~packet(){}

//--------------------------------> the part packet error <------------------------------------//

zeabus_serial::packet_error::packet_error(const packet &data_packet , uint8_t code)
		: std::runtime_error( zeabus_serial::packet_error::generate_string( data_packet,code )){}

std::string zeabus_serial::packet_error::generate_string( const packet &data_packet 
														, uint8_t code ){
		std::stringstream temporary;
		temporary << p.toString();
		return temporary.str();
}

//------------------------------------> the part serial <--------------------------------------//

zeabus_serial::serial::serial() :
		IO_service()
		, serial_port( IO_service )
		, deadline_time( IO_service )
		, byte_transfer( 0 ) 
		, IO_state( IO_done ) {}

zeabus_serial::serial::~serial(){}

bool zeabus_serial::serial::open_port( std::string port
								,	unsigned int baud_rate
								,	boost::asio::serial_port_base::parity opt_parity = 
										boost::asio::serial_port_base::parity( 
											boost::asio::serial_port_base::parity::none
										)
								,	boost::asio::serial_port_base::character_size opt_csize = 
										boost::asio::serial_port_base::character_size( 8 )
								,	boost::asio::serial_port_base::flow_control opt_flow = 
										boost::asio::serial_port_base::flow_control(
											boost::asio::serial_port_base::flow_control::none
										)
								,	boost::asio::serial_port_base::stop_bits opt_stop = 
										boost::asio::serial_port_base::stop_bits(
											boost::asio::serial_port_base::stop_bits::one
										)
								){
	if( port_is_open() ) 
		close_port();

	try{
		serial_port.open( port );
		serial_port.set_option( boost::asio::serial_port_base::baud_rate( baud_rate));
		serial_port.set_option( opt_parity );
		serial_port.set_option( opt_csize );
		serial_port.set_option( opt_flow );
		serial_port.set_option( opt_stop );
	}
	catch ( boost::system::system_error& error){
		std::cout << "Error: " << error.what() << "\n";
		return false;
	}
	return true;
}

void zeabus_serial::serial::close_port(){
	serial_port.close();
}

bool zeabus_serial::serial::port_is_open(){
	return	serial_port::is_open();
}

void zeabus_serial::serial::change_baud_rate( unsigned int baud_rate){
	try{
		serial_port.set_option( boost::asio::serial_port_base::baud_rate( baud_rate ) );
	}
	catch ( boost::system::system_error& error){
		std::cout << "Error: " << error.what() << std::endl;
	}
}

void zeabus_serial::serial::write_packet( const zeabus_serial::packet& data_packet 
										, int time_out ){
	zeabus_serial::data_stream temporary;
	data_packet.get_data_stream( temporary );
	write_data( temporary , timeout);
}

void zeabus_serial::serial::write_string( const std::string& message){
	boost::system::error_code ignored_error;
	boost::asio::write( serial_port , boost::asio::buffer( message )
					,	boost::asio::transfer_all() , ignored_error );
}

std::string zeabus_serial::serial::read_line(){
	std::string line;
	char c;

	for( ; ; ){
		boost::asio::read( serial_port , boost::asio::buffer( &c , 1));
		if( c == '\r')	break;
		else if( c == '\n' ) return line;
		else line += c; 
	}
	return line;
}

void zeabus_serial::serial::write_data( const data_stream& data , int time_out){
	boost::chrono::high_resolution_clock::time_point time_start = 
		boost::chrono::high_resolution_clock::now();

	boost::chrono::high_resolution_clock::time_point time_stop =
		time_start + boost::chrono::milliseconde( time_out );

	size_t written = 0;
	while ( written < data.size() ){
		const ssize_t amont = boost::asio::write( serial_port 
											, boost::asio::buffer(&data[written] , data.size())	
											);
		if( amont > 0) written += amont;
		else if( amont < 0 ){
			if( boost::chrono::errno == EAGAIN || boost::chrono::errno == EINTR ){
				// blocked or interrupted - try again until time out
			}
			else{
				throw boost::chrono::IOError("I/O error while writing");
			}
		}

		if( time_stop < boost::chrono::high_resolution_clock::now() ){
			throw zeabus_serial::timeout_error( true , time_out);
		}	
	}
}

void zeabus_serial::serial::read_handle( const boost::system::error_code& error_code 
										,const size_t bytes_transferred ){
	if( !error_code){
		IO_state = IO_done;
		byte_transfer = bytes_transferred;
		return;
	}

	if( error_code.value() == 125){
		return;
	}

	IO_state = IO_error;
}

void zeabus_serial::serial::timer_handle( const boost::system::error_code& error_code){
	if( !error_code && ( IO_state == IO_process)){
		IO_state = IO_timeout;
	}
}

void zeabus_serial::serial::async_read_block_of_data( data_stream& data, size_t size 
													, int time_out ){
	if( data.size() < size ){
			data.resize( size );
	}

	boost::asio::async_read( serial_port
							,boost::asio::buffer( data , size )
							,boost::bind(&zeabus_serial::serial::read_handle 
										, this 
										, boost::asio::placeholders::error
										, boost::asio::placeholders::bytes_transferred
										)
							);
	deadline_time.expires_from_now( boost::posix_time::milliseconde( time_out ));
	deadline_time.async_wait( boost::bind(&zeabus_serial::serial::timer_handle
										, this
										, boost::asio::placeholders::error
										)
							);

	IO_state = IO_process;
	byte_transfer = 0;

	while( true ){
		IO_service.run_one();
		switch( IO_state){
			case IO_process:
				continue;
			case IO_done:
				deadline_time.cancel();
				return;
			case IO_timeout:
				serial_port.cancel();
				throw zeabus_serial::timeout_error( false , time_out);
			case IO_error:
			default:
				deadline_time.cancel();
				serial_port.cancel();
				throw zeabus_serial::IO_error( "I/O error while reading");
		}
	}	
}
