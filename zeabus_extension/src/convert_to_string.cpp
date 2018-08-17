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

std::string convert::to_string( char data ){
	std::ostringstream temporary;
	temporary << data;
	return temporary.str();
}

std::string convert::edit_space( std::string data , std::string key){
	int order = 0;
	std::string answer = "";
	while( true ){
		if( data[order] == '\n' || data[order] == '\0' ) break;
		else if( in_set( data[order] ) ) answer += key;
		else answer += to_string( data[order] );
		order++;
	}
	return answer;
}

bool convert::in_set( char data){
	int run = 0;
	for( run = 0 ; run < 7 ; run++){
		if( data == set[run]) return true;
	}
	return false;
}
