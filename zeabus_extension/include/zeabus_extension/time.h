/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File's name		: time.h
//
//	Last Update		: Aug 17 , 2018
//	Author			: Supasan Komonlit
//
//	Main purpose	: manage about time
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include	<iostream> // standard library for cpp
#include	<ctime> // use for local time
#include	<boost/date_time/posix_time/conversion.hpp> // for convert type time
#include	<boost/date_time/gregorian/gregorian.hpp> //  not use in this time
#include	<boost/date_time/posix_time/posix_time.hpp> // for world time
#include	<zeabus_extension/convert_to_string.h> // for convert to string
#include	<boost/chrono/duration.hpp> // don't use in this time
#include	<boost/thread/thread.hpp> //  for sleep

#ifndef nullptr
	#define nullptr 0
#endif

//////////////////////////////// Information Addition //////////////////////////////////////////
// type std::time_t

// ------------------------------> for about name file <---------------------------------------
namespace world_time{
	class time{

		public:
			time( bool print_test ); // for function init class
			time();
			std::string local_time(); // return string for local time (computer time)
			std::string universal_time(); // return string for UTC time
			boost::posix_time::ptime local_data; // for collect data of local time
			boost::posix_time::ptime universal_data; // for collect data of universal time
			void get_time();
			void test_diff();

		private:
			bool print_test;
	};
}

// ---------------------------> use in code for calculate <------------------------------------
