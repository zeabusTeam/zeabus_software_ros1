/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File's name		: manage_port.h
//
//	Create on		: Oct 05 , 2018
//	Author			: Supasan Komonlit
//
//	Main purpose	: for using manage port
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef	BOOST_ASIO_HPP
	#include	<boost/asio.hpp> // this is top of serial port I include all
	#define BOOST_ASIO_HPP
#endif

#ifndef BOOST_ASIO_STEADY_TIMER
	#include	<boost/asio/steady_timer.hpp>
	#define BOOST_ASIO_STEADY_TIMER
#endif

#ifndef IOSTREAM
	#include	<iostream>
	#define BOOST_ASIO_HPP
#endif

#ifndef VECTOR
	#include	<vector>
	#define VECTOR
#endif

namespace zeabus_extension{
namespace manage_port{

	size_t num_temp;
	void read_handle( const boost::system::error_code& error , std::size_t bytes_transfer);
	void write_handle( const boost::system::error_code& error , std::size_t bytes_transfer);
	
	class specific_port : private boost::noncopyable{

		public:
			specific_port(  std::string name_port = "" ); //function init
			~specific_port(); // function for delete port

			void set_name_port( std::string name_port ); //set name of port

			void open_port( std::string name_device = ""); // open port

			void close_port(); // we will close port now

			bool is_open(); // check now port are open or not

			// for check option of port // don't use have problem  undefined reference to 
			template<typename port_option> void get_option( port_option& option );

			// for set new option of port // don't use have problem  undefined reference to
			template<typename port_option> void set_option( port_option& option );

			std::string read_string(); // read serial port one line and return in type string

			void write_string( std::string data );//write to serial port receive data only string

			// 5 line below is a port_option
			boost::asio::serial_port_base::baud_rate io_baud_rate;
			boost::asio::serial_port_base::flow_control io_flow_control;
			boost::asio::serial_port_base::parity io_parity;
			boost::asio::serial_port_base::stop_bits io_stop_bits;
			boost::asio::serial_port_base::character_size io_character_size;
			// end of port_option

			// if want to get or set option we use function member of
			// boost::asio::serial_port
			boost::asio::serial_port* io_port;
/////////////////////////////////////////////////////////////////////////////////////////////////
//								NEXT IS ASYNCHRONOUS PART									   //
/////////////////////////////////////////////////////////////////////////////////////////////////
			std::vector<unsigned char> read_asynchronous( size_t bytes_transfer );
//			template<typename buffer_data>void write_asynchronous( buffer_data data 
//												, size_t bytes);	
			void write_asynchronous( std::vector<unsigned uint8_t> data 
												, size_t bytes);	

		protected:
			// order is importance service must build first
			boost::asio::io_service io_service;
			boost::asio::steady_timer* io_time;

		private:
			std::string name_port; // collect name of port to open example is /dev/ttys0
			// this handle is for read and write it is formular from boost
			// when call write or read operation completes.
	};

}
}
