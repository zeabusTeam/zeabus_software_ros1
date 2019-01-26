/*
	File name			:	boost_time.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , JAN 16
	Date last modified	:	2018 , ??? 16
	Purpose				:	For convert time of boost library to string
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Maintain

	Namespace			:	zeabus_library/convert
*/

#include	<boost/date_time/gregorian/gregorian.hpp> 
#include	<boost/date_time/posix_time/posix_time.hpp>

#include	<iostream>

#include	<sstream>

#ifndef _ZEABUS_LIBRARY_CONVERT_BOOST_TIME_H__
#define _ZEABUS_LIBRARY_CONVERT_BOOST_TIME_H__

namespace zeabus_library{

namespace convert{


	std::string time_to_string( boost::posix_time::ptime& data );
	
	void time_to_string( boost::posix_time::ptime& data , std::string& result );

	std::string time_to_string( boost::gregorian::date& data );
	
	void time_to_string( boost::gregorian::date& data , std::string& result );


}

}

#endif
