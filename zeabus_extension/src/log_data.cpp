/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File's name		: log_data.cpp
//
//	Last Update		: Sep 01 , 2018
//	Author			: Supasan Komonlit
//
//	Main purpose	: Detail Function of log_data in log_data.h
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ZEABUS_EXTENSION_LOG_DATA
	#define ZEABUS_EXTENSION_LOG_DATA
	#include	<zeabus_extension/log_data.h>
#endif

zeabus_extension::log_data::log::log( std::string package_name , std::string path 
				,  std::string name_file , bool anonymous ) : 
		zeabus_extension::zeabus_ros::find_path::find_path( package_name , path , name_file ){

	this->only_name = name_file;
	this->package_name = package_name;
	this->path = path;
	this->new_file( package_name , path , only_name , anonymous );
	time = new zeabus_extension::precise_time::millisecond();
//	time.reset_time();

}

void zeabus_extension::log_data::log::new_file( std::string package_name , std::string path 
				, std::string name_file , bool anonymous ) {
	if( name_file != nullstring)	this->only_name = name_file;
	if( package_name != nullstring)	this->package_name = package_name;
	if( path != nullstring )		this->path = path;
	time_file.reset_time();
	if( this->name_file == "~"){
		this->name_file = zeabus_extension::convert::edit_space(
										time_file.local_time() , "_") + ".txt";
	}
	else{
		if( anonymous ){
			this->name_file = only_name + "_" 
							+ zeabus_extension::convert::edit_space(
										time_file.local_time() , "_") + ".txt";
		}
		else{
			this->name_file = only_name + ".txt";
		}
	}
	this->set_path( package_name , path , this->name_file );
	std::string start_text = "------------------------------------->";
	start_text += " start log ";
	start_text += "<-------------------------------------";
	this->write( start_text , false , true);
}

void zeabus_extension::log_data::log::write( std::string message , bool header , bool start ){
	if( header ){
		std::string arrive_message = "echo \"----------------> "
							+ zeabus_extension::convert::edit_space( 
									zeabus_extension::convert::to_string(time.now()) , "_")
							+ " <----------------" + "\" >>"
							+ this->last_path ;
		std::system( arrive_message.c_str() );
	}
	if( start ){
		message = "echo \"" + message + "\" > " + this->last_path;
	}
	else{
		message = "echo \"" + message + "\" >> " + this->last_path;
	}
	std::system( message.c_str() );
}

void zeabus_extension::log_data::log::clear_screen(){
	std::system( "clear" );
}
