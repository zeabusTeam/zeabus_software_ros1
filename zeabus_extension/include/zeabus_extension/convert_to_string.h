/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File's name		: convert_to_string.h
//
//	Last Update		: Aug 17 , 2018
//	Author			: Supasan Komonlit
//
//	Main purpose	:	for convert type to type
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_DATE_TIME_GREGORIAN_GREGORIAN
	#include	<boost/date_time/gregorian/gregorian.hpp> // for include type date
	#define BOOST_DATE_TIME_GREGORIAN_GREGORIAN
#endif

#ifndef BOOST_DATE_TIME_POSIX_TIME_POSIX_TIME
	#include	<boost/date_time/posix_time/posix_time.hpp> // for include type time
	#define BOOST_DATE_TIME_POSIX_TIME_POSIX_TIME
#endif

#ifndef IOSTREAM
	#include	<iostream> // include standard function of c
	#define IOSTREAM
#endif

#ifndef SSTREAM
	#include	<sstream> // for convert to string
	#define SSTREAM
#endif

namespace convert{

//----------------------------------> part to string <-------------------------------------------
// how to do if want to create function?
// can copy and past 5 line of function 
// style to convert is overload function
// change type first argument
// and please add function in ./../../src/convert_to_string.cpp
// can ready use , Supasan Komonlit Aug 16 2018

	static char set[7] = {' ' , ':' , ',' , '-' , '.' , '_' , ';'};

	std::string to_string( boost::posix_time::ptime data );

	std::string to_string( boost::gregorian::date data );

	std::string to_string( int data );

	std::string to_string( float data );

	std::string to_string( char data );

	std::string edit_space( std::string data , std::string key);

	bool in_set( char data);
}
