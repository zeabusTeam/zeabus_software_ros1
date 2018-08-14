/////////////////////////////////////////////////////////////////////////////////
//
//	serial.h
//  Created on : Aug 11 , 2018
//      Author : Supasan
//Follow original of
//  Created on : Nov 17 , 2014
//      Author : Mahisorn
//
/////////////////////////////////////////////////////////////////////////////////

#include	<boost/asio.hpp>	// This library for network and low-level I/O programming
								// and developers wirh a consistent asynchronous model

#include	<boost/chrono.hpp>	// This library for manage about time

#include	<boost/bind.hpp>	// This library for generalization of the standard functions

#define		u8(x)	static_cast<uint8_t>((x))
#define		u16(x)	static_cast<uint16_t>((x))
#define		u32(x)	static_cast<uint32_t>((x))

namespace serial_zeabus{

	typedef	std::vector<uint8_t>	data_stream;

	typedef union{
				struct{
					uint8_t data_0;
					uint8_t	data_1;
				};
			
					uint16_t data;
			}
			data16;

// this class will show about time out when want to connect port
// class inheritance <----------> runtime_error is go to exceptin when error
	class	error_time_out	:	public std::runtime_error{
		public:
			error_time_out( bool is_write , unsigned int time) : 
				std::runtime_error( generate_string( is_write , time) ){}

		private:
			std::string generate_string( bool is_write , unsigned int time ){
				std::stringstream temporary;
				temporary	<< "Time out while " 
							<< (( is_write ) ? "writing" : "reading ")
							<< ".\n"
							<< "Time out limit is "
							<< time
							<< "ms.";
				return	temporary.str();
			}
	}

	class IO_error	:	public	std::runtime_error{
		public:
			io_error( const std::string& description ): std::runtime_error( description ){}
	}

// virtual is have affect about pointer can change destination to same return and name function
	class package{
		public:
			package() {}

			virtual ~package() {}

			virtual	std::string toString() const = 0;
			virtual void get_data_stream( serial_zeabus::data_stream& buffer) const = 0;
	};

	class package_error	:	public	std::runtime_error{
		public:
			package_error( const package &p, uint8_t code) : 
				std::runtime_error( generate_string( p , code) ){}

		private:
			generate_string( const package &p , uint8_t code){
				std::stringstream temporary;
				temporary << p.toString();
				return temporary.str();
		}
	}

// boost::noncopyable is using protectd constructor and desrructor
	class serial : private boost::noncopyable{

		public:
			enum{
				IO_in_process,
				IO_done,
				IO_error,
				IO_timeout,
			};

			serial();

			virtual ~serial();

			bool open_port(	std::string name_port , 
							unsigned int baud_rate, 
							boost::asio::serial_port_base::parity opt_parity =
								boost::asio::serial_port_base::parity( 
									boost::asio::serial_port_base::parity::none),

							boost::asio::serial_port_base::character_size opt_csize = 
								boost::asio::serial_port_base::character_size(8),

							boost::asio::serial_port_base::flow_control opt_flow =
								boost::asio::serial_port_base::flow_control(
									boost::asio::serial_port::flow_control::none),

							boost::asio::serial_port_base::stop_bits opt_stop = 
								boost::asio::serial_port_base::stop_bits(
									boost::asio::serial_port_base::stop_bits::one)
							);

			void close_port();

			bool is_open();

			void change_baud_rate( unsigned int baud_rate);

			// to change to async_write
			void write_package( const serial_zeabus::package& package ,  int timeout);

			void write_string( const std::string& message);

			// to make asyn and timeout supported
			std::string readLine();

			void write_data( const data_stream& data , int timeout );

			virtual void read_handle( const boost::system::error_code& ec ,
									  const size_t bytes_transferred);

			virtual void timer_handle( const boost::system::error_code& ec);

			//asyn read fixed amount of data
			void async_read_block_of_data( data_stream& data , size_t size , int timeout);

		protected:
			boost::asio::io_service IO;
			boost::asio::serial_port port;
			boost::asio::deadline_time timer;
			int byte_transferref;
			int IO_state;
	};

}
