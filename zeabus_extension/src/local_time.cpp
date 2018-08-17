/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File's name		: count_time.cpp
//
//	Last Update		: Aug 17 , 2018
//	Author			: Supasan Komonlit
//
//	Main purpose	: manage about time that want more accuracy
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include	<zeabus_extension/time.h>

count_time::time::time( bool print_test ){
	this->print_test = print_test;
}

count_time::time::time( ){
	this->print_test = false;
}

void count_time::time::reset_time(){
	time_01 = boost::posix_time::second_clock::local_time();
}

void find_different(){
	temporary = boost::posix_time::second_clock::local_time() - time_01;
}

long count_time::second::different( bool reset){
	this->find_different();
	if( reset ) this->reset_time();
	return temporary.total_second();
}

long count_time::millisecond::different( bool reset){
	this->find_different();
	if( reset ) this->reset_time();
	return temporary.total_millisecond();
}

long count_time::microsecond::different( bool reset){
	this->find_different();
	if( reset ) this->reset_time();
	return temporary.total_microsecond();
}

long count_time::nanosecond::different( bool reset){
	this->find_different();
	if( reset ) this->reset_time();
	return temporary.total_nanosecond();
}

