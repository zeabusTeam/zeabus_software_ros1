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
#include	<boost/date_time/posix_time/posix_time.hpp> // for world time
#include	<boost/date_time/posix_time/posix_time_duration.hpp> // for calculate diff
#include	<boost/chrono/system_clocks.hpp> // for accuracy time
#include	<zeabus_extension/convert_to_string.h> // for convert to string

#include	<boost/thread/thread.hpp> //  for sleep

#ifndef nullptr
	#define nullptr 0
#endif

//////////////////////////////// Information Addition //////////////////////////////////////////
// type std::time_t accuracy is second
// protected are according inheritance to access. private can't do that

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

		private:
			bool print_test;
			bool collect_log;
	};
}

// ---------------------------> use in code for calculate <------------------------------------
namespace count_time{

	class time{
		
		public:
			time( bool print_test );
			time();
			void reset_time();
				
		protected:
			boost::posix_time::ptime time_01;
			boost::posix_time::time_duration temporary;
			void find_different();

	}

	class second : public time{
		public:
			long different( bool reset );
	}

	class millisecond : public time{
		public:
			long different( bool reset );
	}

	class microsecond : public time{
		public:
			long different( bool reset );
	}

	class nanosecond : public time{
		public:
			long different( bool reset );
	}

}

namespace timer{
	void sleep( double data );
}
