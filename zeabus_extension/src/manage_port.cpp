/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File's name		: manage_port.cpp
//
//	Create on		: Oct 05 , 2018
//	Author			: Supasan Komonlit
//
//	Main purpose	: detaile of manage_port.h
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ZEABUS_EXTENSION_MANAGE_PORT_H
	#include	<zeabus_extension/manage_port.h>
	#define ZEABUS_EXTENSION_MANAGE_PORT_H
#endif

//#define TEST_MANAGE_PORT
#define TEST_PORT_SCHEDULE

namespace zeabus_extension{

namespace manage_port{

// function of init class
	specific_port::specific_port( std::string name_port )
	{
		this->io_port = new boost::asio::serial_port( this->io_service);
		this->io_time = new boost::asio::steady_timer( this->io_service);
		current_state = free_state;
		if( name_port != "") this->name_port = name_port;
		else this->name_port = "";
	}

// check port now open or not by using boost::asio::basic_serial_port::is_open
	bool specific_port::is_open(){
		// port is variable in protected in this class
		if( this->io_port->is_open() ) return true; 
		else return false;
	}

// this function for destroy port
	specific_port::~specific_port(){
		std::cout << "System from manage_port :: Close port" << "\n";
		current_state = close_state;
		delete this->io_port;
	}

// this function for only open port 
	void specific_port::open_port( std::string name_device ){
		if( this->name_port == ""){
			std::cout << "You never set name of port what you want" << "\n";
			exit(-1);
		}
		// when open port we must to use try to open because it can't error
		// and it make code to run continuouse please check again
		// we not automatical to close program please close by yourself
		try{
			this->io_port->open( this->name_port ); // try to open port
			std::cout << "ZEABUS_EXTENSION : Now Open port " << this->name_port;
			if( name_device == "" ) std::cout << "\n";
			else std::cout << " for " << name_device << "\n";
			current_state = free_state;
		}
		catch ( std::exception& error){
			std::cout	<< error.what() << "\n"; // print error 
			std::cout	<< "ZEABUS_EXTENSION : Can't open port " << this->name_port;
			if( name_device == "" ) std::cout << "\n";
			else std::cout << " for " << name_device << "\n";
		}
	}

// for close port
	void specific_port::close_port(){
		this->io_service.reset();
		this->io_port->close();
	}

// change name of port on this object
	void specific_port::set_name_port( std::string name_port){
		if( this->is_open() ){ // must check we are now open or not
			
		}
		this->name_port = name_port;
	}	

// for get option please remember argument receive in form reference
	template<typename port_option> 
	void specific_port::get_option( port_option& option ){
		this->io_port->get_option( option );	
	}
	 
// for set option please remember argument receive in for rederence
	template<typename port_option>
	void specific_port::set_option( port_option& option ){
		this->io_port->set_option( option );
	}

// for read data
	std::string specific_port::read_string(){
		// create buffer to receive data
		std::vector< char > data_buffer;
		size_t position = 0;

		std::cout << "SYSTEM--> In read_string of specific_port class in file manage_port.cpp";
		std::cout << " start read <----------\n";
		while( true ){
			std::cout << "before read_some \n";
			this->io_port->read_some( boost::asio::buffer( &data_buffer , 1 ));
			std::cout << "data_buffer is " << data_buffer[position] << "\n";
			if( data_buffer[ position ] == '\n' || data_buffer[position] == '\0'){
				break;
			}
			else{
				position++;
			}
		}
		std::cout << "SYSTEM--> In read_string of specific_port class in file manage_port.cpp";
		std::cout <<  " end read <-----------\n";

		std::string result = "";

		std::cout << "SYSTEM--> In read_string of specific_port class in file manage_port.cpp";
		std::cout << " start prepare output <---------\n";
		while ( ! data_buffer.empty()){ // run until buffer is empty
			result = data_buffer.back()	+ result; // prepare output
			data_buffer.pop_back();// remove last element
		}
		std::cout << "SYSTEM--> In read_string of specific_port class in file manage_port.cpp";
		std::cout << " finish prepare output <--------\n";
	}

// for write data
	void specific_port::write_string( std::string data ){
		// create buffer to send and will you with boost::asio::buffer
		std::vector< char > data_buffer;
		
		std::cout << "SYSTEM--> In write_string of specific_port class in file manage_port.cpp";
		std::cout << "  start write <-------\n";
		size_t data_size = 0;
		for( int run = 0 ; data[run] != '\n' && data[run] != '\0' ; run++){
			data_buffer.push_back( data[run] ); // get data[run] in to buffer char type
			data_size++; // plus size of data
		}
		std::cout << "SYSTEM--> Finish get data in to buffer and have size is ";
		std::cout << data_size << "\n";

		size_t data_write = this->io_port->write_some( 
								boost::asio::buffer(&data_buffer , data_size) 
							);
		
//		boost::asio::write( this->io_port , boost::asio::buffer( data_buffer , data_size));
		
		std::cout << "SYSTEM--> FINISH WRITE PORT and have size is " << data_write  << "\n";
	}
//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//	FINISH PART OF SYNCHRONOUS , I THOUGH THAT NEXT IS PART OF ASYNCHRONOUS FOR SEND PACKET //
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
		void specific_port::read_handle( const boost::system::error_code& error 
						   , std::size_t bytes_transfer ){
			#ifdef TEST_MANAGE_PORT
				std::cout	<< "<--SYSTEM--> IN read_handler " 
//							<< "size of vector is " << buffers
							<< "error code : " << error
							<< " and bytes_transfer is " << bytes_transfer
//							<< " want byte is " << number_bytes
							<< "\n";
			#endif
			#ifdef TEST_PORT_SCHEDULE
				std::cout	<< "finish read_handle\n";
			#endif
			current_state = done_state;
		}

		void specific_port::write_handle( const boost::system::error_code& error 
							, std::size_t bytes_transfer){
			#ifdef TEST_MANAGE_PORT
				std::cout	<< "<--SYSTEM--> In write handle "
							<< "  error code : " << error
							<< "have bytes_transfer is " << bytes_transfer
							<< "\n";
			#endif
			#ifdef TEST_PORT_SCHEDULE
				std::cout	<< "finish write_handle\n";
			#endif
			current_state = done_state;
		}

		void specific_port::read_asynchronous( size_t number_bytes 
												, std::vector<uint8_t>&data_receive){
			data_receive.resize(number_bytes);

			#ifdef TEST_MANAGE_PORT
				std::cout	<< "SYSTEM----> Before async_read_some number_bytes is " 
							<< number_bytes << "\n";
			#endif
			// push queqe for read data			
/*			this->io_port->async_read_some( boost::asio::buffer( data_receive , number_bytes),
										read_handler);*/
			// use function async_read because want to ensure have read equal data to request
			boost::asio::async_read( *(this->io_port) 
				, boost::asio::buffer( data_receive , number_bytes)
				, boost::asio::transfer_exactly( number_bytes)
				, boost::bind( &zeabus_extension::manage_port::specific_port::read_handle , this 
						, boost::asio::placeholders::error 
						, boost::asio::placeholders::bytes_transferred)
			);
			

			#ifdef TEST_MANAGE_PORT
				std::cout	<< "SYSTEM----> AFTER async_read_some\n"
							<< "----------------> NEXT RUNS io_service.run_for_one\n";
			#endif

			// run one queqe
			#ifdef TEST_PORT_SCHEDULE
				std::cout << "<--SCHEDULE--> Before into loop in function read\n";
			#endif
			while( current_state != free_state || current_state == done_state){
				switch( current_state ){
					case free_state		:	std::cout << "Should out of loop\n";
											break;
					case process_state	:	std::cout << "Wait for other porcess\n";
											break;
					case close_state	:	std::cout << "Port isn't open\n";
											exit( -2);
											break;
					case done_state		:	std::cout << "Wait other process reset\n";
											break;
				}
			}
			#ifdef TEST_PORT_SCHEDULE
				std::cout << "<--SCHEDULE--> After loop in read function\n";
			#endif
			current_state = process_state;
			this->io_service.run_one();
			#ifdef TEST_PORT_SCHEDULE
				std::cout << "<--SCHEDULE--> Finish run one in read function\n";
			#endif

			#ifdef TEST_MANAGE_PORT
				std::cout	<< "SYSTEM----> AFTER RUN io_service "
							<< "size of vector is " << data_receive.size()
							<< "\n";
			#endif
//			while( current_state != done_state ){}
			#ifdef TEST_PORT_SCHEDULE
				std::cout << "<--SCHEDULE--> Out of read_handle go next queqe\n";
			#endif	
			this->io_service.reset();
			current_state = free_state;

		}

//		template<typename buffer_data>void specific_port::write_asynchronous( buffer_data data 
//												, size_t bytes){
		void specific_port::write_asynchronous( std::vector<uint8_t> data 
												, size_t bytes){
			#ifdef TEST_MANAGE_PORT
				std::cout	<< "SYSTEM-----> Before async_write bytes to write is "
							<< bytes << "\n";
			#endif
			// use function async_write because want to ensure all data is written
			boost::asio::async_write( *(this->io_port)
				, boost::asio::buffer( data , bytes)
				, boost::asio::transfer_all()
				, boost::bind( &zeabus_extension::manage_port::specific_port::write_handle, this 
						, boost::asio::placeholders::error 
						, boost::asio::placeholders::bytes_transferred)
			);	
			
			#ifdef TEST_MANAGE_PORT
				std::cout	<< "SYSTEM-----> After async_write and next run io_service\n";
			#endif
			#ifdef TEST_PORT_SCHEDULE
				std::cout << "<--SCHEDULE--> Before into loop in function write\n";
			#endif

			while( current_state != free_state || current_state == done_state){
				switch( current_state ){
					case free_state		:	std::cout << "Should out of loop\n";
											break;
					case process_state	:	std::cout << "Wait for other porcess\n";
											break;
					case close_state	:	std::cout << "Port isn't open\n";
											break;
					case done_state		:	std::cout << "Wait other process reset\n";
											break;
				}
			}
			#ifdef TEST_PORT_SCHEDULE
				std::cout << "<--SCHEDULE--> After loop in function write \n";
			#endif
			current_state = process_state;
			this->io_service.run_one();
			#ifdef TEST_PORT_SCHEDULE
				std::cout << "<--SCHEDULE--> Finish run one in function write \n";
			#endif

			#ifdef TEST_MANAGE_PORT
				std::cout	<< "SYSTEM-----> Finish run\n";
			#endif
//			while( current_state != done_state ){}
			#ifdef TEST_PORT_SCHEDULE
				std::cout << "<--SCHEDULE--> Finish handle in function write  go to next\n";
			#endif
			this->io_service.reset();
			current_state = free_state;

		}

}
}
