/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File's name		: precise_time.cpp
//
//	Last Update		: Aug 31 , 2018
//	Author			: Supasan Komonlit
//
//	Main purpose	: manage about time that want more precise data up to 6 position
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ZEABUS_EXTENSION_TIME
	#include	<zeabus_extension/time.h>
	#define ZEABUS_EXTENSION_TIME
#endif

zeabus_extension::precise_time::time::time( bool print_test ){
	this->print_test = print_test;
}

boost::posix_time::ptime zeabus_extension::precise_time::time::now(){
	return boost::posix_time::microsec_clock::local_time();
}

void zeabus_extension::precise_time::time::reset_time(){
	time_01 = boost::posix_time::microsec_clock::local_time();
	if( print_test ) std::cout << "now time_01 is " << time_01 << "\n" ;
}

void zeabus_extension::precise_time::time::find_different(){
	temporary = boost::posix_time::microsec_clock::local_time() - time_01;
}

long zeabus_extension::precise_time::second::raw_different( bool reset){
	this->find_different();
	if( reset ) this->reset_time();
	return temporary.total_seconds();
}

long zeabus_extension::precise_time::millisecond::raw_different( bool reset){
	this->find_different();
	if( reset ) this->reset_time();
	return temporary.total_milliseconds();
}

long zeabus_extension::precise_time::microsecond::raw_different( bool reset){
	this->find_different();
	if( reset ) this->reset_time();
	return temporary.total_microseconds();
}

long zeabus_extension::precise_time::nanosecond::raw_different( bool reset){
	this->find_different();
	if( reset ) this->reset_time();
	return temporary.total_nanoseconds();
}

double zeabus_extension::precise_time::second::different( bool reset){
	return this->raw_different( reset );
}

double zeabus_extension::precise_time::millisecond::different( bool reset){
	return 1.0 * this->raw_different( reset ) / 1000;
}

double zeabus_extension::precise_time::microsecond::different( bool reset){
	return 1.0 * this->raw_different( reset ) / 1000000;
}

double zeabus_extension::precise_time::nanosecond::different( bool reset){
	return 1.0 * this->raw_different( reset ) / 1000000000;
}

void zeabus_extension::precise_time::second::sleep( double data ){
	if( print_test) std::cout << "second sleep is " << data*1000 << " ms \n";
	boost::this_thread::sleep( boost::posix_time::millisec( data * 1000 ) );
}

void zeabus_extension::precise_time::millisecond::sleep( double data ){
	if( print_test) std::cout << "millisecond sleep is " << data <<  " ms \n";
	boost::this_thread::sleep( boost::posix_time::millisec( data ) );
}

void zeabus_extension::precise_time::microsecond::sleep( double data ){
	if( print_test) std::cout << "microsecond sleep is " << data << " us\n";
	boost::this_thread::sleep( boost::posix_time::microsec( data ) );
}

void zeabus_extension::precise_time::nanosecond::sleep( double data ){
	if( print_test) std::cout << "nanosecond sleep is " << data * 1000 << " us\n";
	boost::this_thread::sleep( boost::posix_time::microsec( data * 1000 ) );
}
