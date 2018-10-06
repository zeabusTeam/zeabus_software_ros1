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
	specific_port::specific_port( std::string name_port )
	{
		this->io_port = new boost::asio::serial_port( this->io_service);
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

}
}
