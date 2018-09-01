/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File's name		: generate_message.h
//
//	Last Update		: Aug 24 , 2018
//	Author			: Supasan Komonlit
//
//	Main purpose	: for create message to log
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ZEABUS_EXTENSION_CONVERT_TO_STRING
	#include	<zeabus_extension/convert_to_string.h>
	#define ZEABUS_EXTENSION_CONVERT_TO_STRING
#endif

#ifndef	IOSTREAM
	#include	<iostream>
	#define IOSTREAM
#endif

#ifndef	NULLPTR
	#define nullptr 0
#endif

namespace zeabus_extension{
	namespace generate_message{

		std::string specific_message( int number_line = 0 
									, std::string* message = nullptr 
									, std::string* detail = nullptr );

		class log_message{

			public:
				log_message( int number_line = 0 , std::string *message = nullptr);
				void setup_begin( std::string* message = nullptr , int number_line = 0);
				std::string message( std::string* data);
			
			protected:
				int number_line;
				std::string* begin_line;

		};

	}
}
