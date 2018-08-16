/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File's name		: convert_to_string.h
//
//	Last Update		: Aug 16 , 2018
//	Author			: Supasan Komonlit
//
//	Main purpose	:	for convert type to type
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include	<boost/date_time/gregorian/gregorian.hpp> // for include type date
#include	<boost/date_time/posix_time/posix_time.hpp> // for include type time

#include	<iostream> // include standard function of c
#include	<sstream> // for convert to string

namespace convert{

//----------------------------------> part to string <-------------------------------------------
// how to do if want to create function?
// can copy and past 5 line of function 
// style to convert is overload function
// change type first argument
// can ready use , Supasan Komonlit Aug 16 2018

	std::string to_string( boost::posix_time::ptime data ){
		std::ostringstream temporary;
		temporary << data;
		return temporary.str();
	}

	std::string to_string( boost::gregorian::date data ){
		std::ostringstream temporary;
		temporary << data;
		return temporary.str();
	}

	std::string to_string( int data ){
		std::ostringstream temporary;
		temporary << data;
		return temporary.str();
	}

	std::string to_string( float data ){
		std::ostringstream temporary;
		temporary << data;
		return temporary.str();
	}
}
