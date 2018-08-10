/////////////////////////////////////////////////////////////////////////////////
//
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

}
