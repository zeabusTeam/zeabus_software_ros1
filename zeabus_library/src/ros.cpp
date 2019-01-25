/*
	File name			:	ros.cpp
	Author				:	Supasan Komonlit
	Date created		:	2019 , JAN 25
	Date last modified	:	2018 , ??? ??
	Purpose				:	Helping function in ros system
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Production

	Namespace			:	zeabus_library
*/

#include	<zeabus_library/ros.h>

namespace zeabus_library{

	DynParam::DynParam(){
		this->updated = false;
	}

	size_t DynParam::set_package_file( std::string package ){
		this->location_file.package = ros::package::getPath( package );
		this->updated = false;
		return NO_ERROR;
	}

	size_t DynParam::set_directory_file( std::string directory ){
		this->location_file.subdirectory = directory;
		this->updated = false;
		return NO_ERROR;
	}

	size_t DynParam::set_name_file( std::string name ){
		this->location_file.name_file = name + ".yaml";
		this->updated = false;
		return NO_ERROR;
	}

	size_t DynParam::set_node( std::string node ){
		this->node_name = node;
		return NO_ERROR;
	}

	void DynParam::update_directory(){
		this->location_file.result = this->location_file.package + "/"
									+this->location_file.subdirectory + "/"
									+this->location_file.name_file;
		this->updated = true;
	}

	size_t DynParam::dump(){
		if( !(this->updated ) ) this->update_directory();
		std::string command = "rosrun dynamic_reconfigure dynparam dump " + this->node_name
							+ " " + this->location_file.result + " &"; 
	}

	size_t DynParam::load(){
		if( !(this->updated ) ) this->update_directory();
		std::string command = "rosrun dynamic_reconfigure dynparam load " + this->node_name
							+ " " + this->location_file.result + " &"; 
	}

}
