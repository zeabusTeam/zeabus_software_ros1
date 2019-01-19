/*
	File name			:	error_code.cpp
	Author				:	Supasan Komonlit
	Date created		:	2018 , JAN 19
	Date last modified	:	2018 , JAN 19
	Purpose				:	Library for easy to print color
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Production

	Namespace			:	zeabus_library
*/

#include	<zeabus_library/text_color.h>

namespace zeabus_library{

	void normal_red( std::string message ){
		printf( NRE "%s" RES , message.c_str() );
	}

	void bold_red( std::string message ){
		printf( BRE "%s" RES , message.c_str() );
	}

	void normal_green( std::string message ){
		printf( NGR "%s" RES , message.c_str() );
	}

	void bold_green( std::string message ){
		printf( BGR "%s" RES , message.c_str() );
	}

	void normal_yellow( std::string message ){
		printf( NYE "%s" RES , message.c_str() );
	}

	void bold_yellow( std::string message ){
		printf( BYE "%s" RES , message.c_str() );
	}

	void normal_blue( std::string message ){
		printf( NBL "%s" RES , message.c_str() );
	}

	void bold_blue( std::string message ){
		printf( BBL "%s" RES , message.c_str() );
	}

	void normal_magenta( std::string message ){
		printf( NMA "%s" RES , message.c_str() );
	}

	void bold_magenta( std::string message ){
		printf( BMA "%s" RES , message.c_str() );
	}

}
