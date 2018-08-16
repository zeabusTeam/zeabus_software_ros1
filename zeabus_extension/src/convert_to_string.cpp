/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File's name		: convert_to_string.cpp
//
//	Last Update		: Aug 16 , 2018
//	Author			: Supasan Komonlit
//
//	Main purpose	:	for convert type to type
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include	<zeabus_extension/convert_to_string.h> // include standard function of c

std::string convert::to_string( boost::posix_time::ptime data ){
		std::ostringstream temporary;
		temporary << data;
		return temporary.str();
}

std::string convert::to_string( boost::gregorian::date data ){
		std::ostringstream temporary;
		temporary << data;
		return temporary.str();
}

std::string convert::to_string( int data ){
		std::ostringstream temporary;
		temporary << data;
		return temporary.str();
}

std::string convert::to_string( float data ){
		std::ostringstream temporary;
		temporary << data;
		return temporary.str();
}

