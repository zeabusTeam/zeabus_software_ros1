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
	time_01 = boost::posix_time::microsec_clock::local_time();
	if( print_test ) std::cout << "now time_01 is " << time_01 << "\n" ;
}

void count_time::time::find_different(){
	temporary = boost::posix_time::microsec_clock::local_time() - time_01;
}

long count_time::second::different( bool reset){
	this->find_different();
	if( reset ) this->reset_time();
	return temporary.total_seconds();
}

long count_time::millisecond::different( bool reset){
	this->find_different();
	if( reset ) this->reset_time();
	return temporary.total_milliseconds();
}

long count_time::microsecond::different( bool reset){
	this->find_different();
	if( reset ) this->reset_time();
	return temporary.total_microseconds();
}

long count_time::nanosecond::different( bool reset){
	this->find_different();
	if( reset ) this->reset_time();
	return temporary.total_nanoseconds();
}

void count_time::second::sleep( double data ){
	if( print_test) std::cout << "second sleep is " << data*1000 << " ms \n";
	boost::this_thread::sleep( boost::posix_time::millisec( data * 1000 ) );
}

void count_time::millisecond::sleep( double data ){
	if( print_test) std::cout << "millisecond sleep is " << data <<  " ms \n";
	boost::this_thread::sleep( boost::posix_time::millisec( data ) );
}

void count_time::microsecond::sleep( double data ){
	if( print_test) std::cout << "microsecond sleep is " << data << " us\n";
	boost::this_thread::sleep( boost::posix_time::microsec( data ) );
}

void count_time::nanosecond::sleep( double data ){
	if( print_test) std::cout << "nanosecond sleep is " << data * 1000 << " us\n";
	boost::this_thread::sleep( boost::posix_time::microsec( data * 1000 ) );
}

count_time::second::second( bool print_test ){
	this->print_test = print_test;
}

count_time::second::second( ){
	this->print_test = false;
}

count_time::millisecond::millisecond( bool print_test ){
	this->print_test = print_test;
}

count_time::millisecond::millisecond( ){
	this->print_test = false;
}

count_time::microsecond::microsecond( bool print_test ){
	this->print_test = print_test;
}

count_time::microsecond::microsecond( ){
	this->print_test = false;
}

count_time::nanosecond::nanosecond( bool print_test ){
	this->print_test = print_test;
}

count_time::nanosecond::nanosecond( ){
	this->print_test = false;
}

