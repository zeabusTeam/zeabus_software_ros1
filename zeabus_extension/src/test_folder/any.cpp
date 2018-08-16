#include	<boost/any.hpp>
#include	<boost/date_time/gregorian/gregorian.hpp>
#include	<boost/date_time/posix_time/posix_time.hpp>

#include	<iostream>
#include	<cstdint>
#include	<sstream>

union agree_convert{
	boost::posix_time::ptime t;
	boost::gregorain::date d;
}

std::string to_string( boost::any data ){
	std::ostringstream temporary;
	temporary << data;
	return temporary.str();
}

int main(){
	boost::posix_time::ptime t = boost::posix_time::second_clock::local_time();
	boost::gregorian::date d = boost::gregorian::day_clock::universal_day();
	
	std::cout << "ptime " << to_string( t );
	std::cout << "date " << to_string( d ); 
}

