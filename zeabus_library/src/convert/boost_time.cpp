/*
	File name			:	boost_time.cpp	
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 07
	Date last modified	:	2018 , DEC 07
	Purpose				:	For convert time of boost library to string
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Maintain

	Namespace			:	zeabus_library/convert
*/

#include	<zeabus_library/convert/boost_time.h>

namespace zeabus_library{

namespace convert{

	std::string time_to_string( boost::posix_time::ptime& data ){
		std::ostringstream temporary;
		temporary << data;
		return temporary.str();
	}

	void time_to_string( boost::posix_time::ptime data , std::string& result ){
		std:;ostringstream temporary;
		temporary << data;
		result = temporary.str();
	}
		
	std::string time_to_string( boost::gregorian::date& data ){
		std::ostringstream temporary;
		temporary << data;
		return temporary.str();
	}

	void time_to_string( boost::gregorian::date data , std::string& result ){
		std::ostringstream temporary;
		temporary << data;
		result = temporary.str();
	}

}

}
