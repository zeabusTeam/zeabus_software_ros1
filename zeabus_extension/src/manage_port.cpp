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


namespace zeabus_extension{
namespace manage_port{

// function of init class
	specific_port::specific_port( std::string name_port ):
		io_port( io_service )
	{
		if( name_port != "") this->name_port = name_port;
		else this->name_port = "";
	}

// check port now open or not by using boost::asio::basic_serial_port::is_open
	bool specific_port::is_open(){
		if( this->io_port.is_open() ) return true; // port is variable in protected in this class
		else return false;
	}

	void specific_port::open_port( std::string name_device ){
		if( this->name_port == ""){
			std::cout << "You never set name of port what you want" << "\n";
			exit(-1);
		}
		try{
			this->io_port.open( this->name_port ); // try to open port

			std::cout << "ZEABUS_EXTENSION : Now Open port " << this->name_port;
			if( name_device == "" ) std::cout << "\n";
			else std::cout << " for " << name_device << "\n";
		}
		catch ( std::exception& error){
			std::cout	<< error.what() << "\n"; // print error 
			std::cout	<< "ZEABUS_EXTENSION : Can't open port " << this->name_port;
			if( name_device == "" ) std::cout << "\n";
			else std::cout << " for " << name_device << "\n";
		}
	}

	void specific_port::close_port(){
		this->io_port.close();
	}

// change name of port on this object
	void specific_port::set_name_port( std::string name_port){
		if( this->is_open() ){ // must check we are now open or not
			
		}
		this->name_port = name_port;
	}	

}
}
