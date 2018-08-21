/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File's name		: log_data.cpp
//
//	Last Update		: Aug 22 , 2018
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
				,  std::string name_file , bool anonymous ) : 
		ros::find_path::find_path( package_name , path , name_file ){
	this->name_file = name_file;
	this->new_file( package_name , path , name_file , anonymous );
	time = new count_time::time();
}

void log_data::log::new_file( std::string package_name , std::string path 
				, std::string name_file , bool anonymous ){
	time_file.get_time();
	if( this->name_file == "~"){
		this->name_file = convert::edit_space(time_file.local_time() , "_") + ".txt";
	}
	else{
		if( anonymous ){
			this->name_file += "_" + convert::edit_space(time_file.local_time() , "_") + ".txt";
		}
		else{
			this->name_file += ".txt";
		}
	}
	this->set_path( package_name , path , this->name_file );
}

void log_data::log::write( std::string message , bool header){
	if( header){
		std::string arrive_message = "echo \"----------------> "
//						+ "----------> " 
							+ convert::edit_space( convert::to_string(time.now()) , "_")
							+ " <----------------" + "\""
							+ this->last_path ;
		std::system( arrive_message.c_str() );
	}
	message = "echo \"" + message + "\" " + this->last_path;
	std::system( message.c_str() );
}

void log_data::log::clear_screen(){
	std::system( "clear" );
}
