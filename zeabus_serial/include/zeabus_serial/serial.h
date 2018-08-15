/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File	: serial.h
//
//	Edit	: Aug 15 , 2018
//	Author	: Supasan Komonlit
//
//	Thanks	: Mahisorn Nov 17 , 2014
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include	<boost/asio.hpp>
#include	<boost/chrono.hpp>
#include	<boost/bind.hpp>

#define		u8(x)	static_cast< uint8_t >((x))
#define		u16(x)	static_cast< uint16_t >((x))
#define		u32(x)	static_cast< uint32_t >((x))

namespace	zeabus_serial{

	typedef	std::vector<uint8_t> data_stream;

	typedef union { struct {
						uint8_t data_0;
						uint8_t data_1;
					};
					uint16_t data;
	}	data_16

//---------------------------------> class time error <----------------------------------------//
	class	timeout_error : public std::runtime_error{

		public:

			timeout_error( bool is_write , unsigned int time); 
			
		private:

			std::string generate_string( bool is_write , unsigned int time);
	};

//----------------------------------> class IO error <-----------------------------------------//
	class	IO_error : public std::runtime_error{

		public:

			IO_error( const std::string& description );
	};

//------------------------------------> class packet <-----------------------------------------//
	class	packet{

		public:
			packet();

			virtual ~packet();

			virtual std::string to_string() const = 0;

			virtual void get_data_stream( zeabus_serial::data_stream& buffer ) const = 0;
	};

//---------------------------------> class packet error <--------------------------------------//
	class	packet_error{

		public:
			packet_error( const packet &data_packet , uint8_t code);

		private:
			std::string generate_string( const packet &data_packet ,  uint8_t code);
	}

//------------------------------------> class serial <-----------------------------------------//
	class	serial : private boost::noncopyable{
		public:
			enum{
				IO_process,
				IO_done,
				IO_error,
				IO_timeout,
			};
	
			serial();

			virtual ~serial();
		
			bool open_port(	std::string port
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
			);

			void close_port();

			bool port_is_open();

			void change_baud_rate( unsigned int baud_rate);

			void write_packet( const zeabus_serial::packet& data_packet , int time_out);

			void write_string( const std::string& message);

			std::string read_line();

			void write_data( const data_stream& data , int time_out);

			virtual void read_handle(	const boost::system::error_code& message 
									,	const size_t bytes_transferred );

			virtual	void timer_handle( const boost::system::error_code& message)

			void async_read_block_of_data( data_stream& data , size_t size , int time_out);

		protected:
			boost::asio::io_service IO_service;
			boost::asio::serial_port serial_port;
			boost::asio::deadline_time deadline_time;
			int byte_transfer;
			int IO_state;
	};
}
