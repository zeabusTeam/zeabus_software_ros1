/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File's name		: ros.cpp
//
//	Last Update		: Aug 21 , 2018
//	Author			: Supasan Komonlit
//
//	Main purpose	: Detail Function of ros in log_data.h
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ZEABUS_EXTENSION_LOG_DATA
	#define ZEABUS_EXTENSION_LOG_DATA
	#include	<zeabus_extension/log_data.h>
#endif

ros::find_path::find_path( std::string package_name , std::string path 
				, std::string name_file ){
	this->set_path( package_name , path , name_file );
}


void ros::find_path::set_path(  std::string package_name , 	std::string path,
					std::string name_file ){

	std::string package_path = ros::package::getPath( package_name );
	this->last_path = package_path + "/" + path + "/"+ name_file;

}

ros::dynamic_reconfigure::dynamic_reconfigure( std::string package_name , std::string path 
						, std::string name_file , std::string node_name){
	this->set_path( package_name , path , name_file );
	this->set_parameter( node_name );
}

void ros::dynamic_reconfigure::set_parameter( std::string node_name ){
	this->node_name = node_name;
}

void ros::dynamic_reconfigure::save(){
	std::string cmd_string = "rosrun dynamic_reconfigure dynparam dump /" + this->node_name
						+ " " + this->last_path + "&";
	std::system( cmd_string.c_str() );
}

void ros::dynamic_reconfigure::load(){
	std::string cmd_string = "rosrun dynamic_reconfigure dynparam load /" + this->node_name
						+ " " + this->last_path + "&";
	std::system( cmd_string.c_str() );
}
