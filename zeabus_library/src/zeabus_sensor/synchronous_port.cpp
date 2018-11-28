/*
	File name			:	synchronous_port.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2018 , NOV 26
	Date last modified	:	2018 , ??? ??
	Purpose				:	This is source of file abount port style synchronous

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.0.1
	status				:	Production

	Namespace			:	zeabus_sensor
*/

#include	<zeabus_library/zeabus_sensor/synchronous_port.h>

#define DEBUG_CODE

namespace zeabus_sensor{

	SynchronousPort::SynchronousPort( std::string name_port ):
		SerialPort( name_port ){}

	SynchronousPort::~SynchronousPort(){
		this->is_open( temp_boolean );
		if( temp_boolean ){
			this->close_port( temp_boolean );
			printf("Port < %s > now close\n" , this->name_port.c_str() );
		}
		else{
			printf("Port < %s > never open\n" , this->name_port.c_str() );
		}
	}

	size_t SynchronousPort::read_data( std::vector<uint8_t>& buffer , size_t size ){
		size_t read_size = this->io_port.read_some( boost::asio::buffer( buffer , size ) 
												, this->error_code );
		if( this->error_code == errc::success ){
			#ifdef DEBUG_CODE
				printf( "SUCCESS read data < %zd > size want < %zd >\n" , read_size , size );
			#endif	
			return read_size;	
		}	
		return read_size; 
	}

	size_t SynchronousPort::write_data( std::vector<uint8_t>& buffer , size_t size ){
		size_t write_size = 0; 

		while( write_size != size ){
			write_size = this->io_port.write_some( boost::asio::buffer( buffer , size )
												, this->error_code );
		}
		if( this->error_code == errc::success ){
			#ifdef DEBUG_CODE
				printf( "SUCCESS write data < %zd > size\n" , write_size );
				return write_size;
			#endif
		}
		return write_size;
	}

}
