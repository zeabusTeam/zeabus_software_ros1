/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File's name		: log_data.cpp
//
//	Last Update		: Aug 21 , 2018
//	Author			: Supasan Komonlit
//
//	Main purpose	: Detail Function of log_data in log_data.h
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ZEABUS_EXTENSION_LOG_DATA
	#define ZEABUS_EXTENSION_LOG_DATA
	#include	<zeabus_extension/log_data.h>
#endif

log_data::log::log( std::string package_name , std::string path 
			,  std::string name_file , bool anonymous ){
	this->name_file = name_file;
	this->new_file( anonymous );
}

void log_data::log::new_file( bool anonymous ){
	time_file.get_time();
	if( this->name_file == "~"){
		this->name_file = time_file.local_time() + ".txt";
	}
	else{
		if( anonymous ){
			this->name_file += "_" + time_file.local_time() + ".txt";
		}
		else{
			this->name_file += ".txt";
		}
	}
	this->set_path( package_name , path , this->name_file );
}

void log_data::log::write( std::string message ){
	std::string arrive_message = "echo \""
						+ "----------> " + time::now() 
						+ " <----------" + "\""
						+ this->last_path ;
	message = "echo \"" + message + "\" " + this->last_path;
	std::system( arrive_message.c_str() );
	std::system( message.c_str() );
}

void log_data::log::clear_screen(){
	std::system( "clear" );
}
