/*
	File name			:	general.cpp	
	Author				:	Supasan Komonlit
	Date created		:	2018 , JAN 16
	Date last modified	:	2018 , ??? ??
	Purpose				:	For convert or edit string
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Maintain

	Namespace			:	zeabus_library/convert
*/

#include	<zeabus_library/convert/general.h>

namespace zeabus_library{

namespace convert{

	std::string edit_string( std::string& problem , std::string key ){
		std::string result;
		edit_string( problem , key , result );
		return result;
	}

	void edit_string( std::string& problem , std::string key , std::string& result ){
		result = "";
		int order = 0;
		while( true ){
			if( data[order] == '\0' ) break;
			else if( check_syntax( data[ order ] ) ) result += key;
			else result += to_string( data[ order ] );
		}
	}

	std::string to_string( char data ){
		std::ostringstream temporary;
		temporary << data;
		return temporary.str();
	}

	std::string to_string( int data ){
		std::ostringstream temporary;
		temporary << data;
		return temporary.str();
	}

	bool check_syntax( char data ){
		int run = 0 ;
		for( run = 0 ; run < 7 ; run++ ){
			if( data == set_syntax[run] ) return true;
		}
		return false;
	} 
}
}
