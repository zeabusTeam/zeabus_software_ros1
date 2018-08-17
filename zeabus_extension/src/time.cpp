/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File's name		: time.cpp
//
//	Last Update		: Aug 17 , 2018
//	Author			: Supasan Komonlit
//
//	Main purpose	: manage about time
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include	<zeabus_extension/time.h>

world_time::time::time( bool print_test){
	this->print_test = print_test;
	this->get_time();
}

world_time::time::time(){
	this->print_test = false;
	this->get_time();
}

std::string world_time::time::local_time(){
	return convert::to_string( local_data );	
}

std::string world_time::time::universal_time(){
	return convert::to_string( universal_data );
}

void world_time::time::get_time(){
	local_data = boost::posix_time::second_clock::local_time();
	universal_data = boost::posix_time::second_clock::universal_time();
}

local_time::time::time( bool print_test){
	this->print_test = print_test;
	reset_time();
}

local_time::time::time(){
	this->print_test = false;
	reset_time();
}

void local_time::time::reset_time(){
	std::time( &time_01);
}

double local_time::time::diff_reset(){
	this->result = difftime( std::time( nullptr ) , time_01);
	std::time( &time_01);
	return this->result;
}

double local_time::time::diff(){
	return difftime( std::time( nullptr ) , time_01);
}
