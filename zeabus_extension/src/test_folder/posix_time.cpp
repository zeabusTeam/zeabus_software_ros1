#include	<boost/date_time/gregorian/gregorian.hpp>
#include	<boost/date_time/posix_time/posix_time.hpp>
#include	<iostream>
#include	<sstream>

int main(){
	boost::posix_time::ptime t = boost::posix_time::second_clock::local_time();
	std::cout << "test" << t << "\n";	
	boost::gregorian::date d = boost::gregorian::day_clock::universal_day();
	boost::posix_time::ptime data( d );
	std::cout << "data " << data << "\n";
	std::ostringstream temporary;
	temporary << t ;
	std::string test = "data is " + temporary.str();
	std::cout << test << "\n";
}
