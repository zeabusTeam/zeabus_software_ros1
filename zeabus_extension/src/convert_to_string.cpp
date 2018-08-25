/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File's name		: convert_to_string.cpp
//
//	Last Update		: Aug 24 , 2018
//	Author			: Supasan Komonlit
//
//	Main purpose	:	for convert type to type
//
/////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ZEABUS_EXTENSION_CONVERT_TO_STRING
	#include	<zeabus_extension/convert_to_string.h> // include standard function of c
	#define ZEABUS_EXTENSION_CONVERT_TO_STRING
#endif

#define convert_test

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

std::string convert::to_string( int data , bool assign_position , int position , bool sign ){
	std::ostringstream temporary;
	temporary << data;
	if( ! assign_position ) return temporary.str();
	else{
		#ifdef convert_test
			std::cout << "before test in convert\n";
		#endif

		int count = 0;
		std::string problem = temporary.str();
		std::string answer = "";
		for( int run = 0 ; ; run++){
			try{
				if( problem[run] == '+' ) sign = true;
				else if( problem[run] == '-' ) sign = false;
				else if( problem[run] != '\0' && problem[run] != '\n'){
					answer += to_string( problem[run]);
					count++; 
				}
				else break;
			}
			catch( std::exception& error_msg){
				std::cout << "Standard Exception : " << error_msg.what() << "\n";
			}
		}
		std::cout << "In put have all char is " << count << "\n";
		for( ; count != position ; count++){
			answer = "0" + answer;
		}
		if( sign ) answer = "+" + answer;
		else answer = "-" + answer;
		std::cout << "Out put is answer :" << answer << "\n";
		return answer;
	}
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

std::string convert::to_string( double data ){
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
