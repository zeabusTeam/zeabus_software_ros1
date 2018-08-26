/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File's name		: time.h
//
//	Last Update		: Aug 24 , 2018
//	Author			: Supasan Komonlit
//
//	Main purpose	: manage about time
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef IOSTREAM
	#include	<iostream> // standard library for cpp
	#define IOSTREAM
#endif

#ifndef CTIME
	#include	<ctime> // use for local time
	#define CTIME
#endif

#ifndef BOOST_DATE_TIME_POSIX_TIME_POSIX_TIME
	#include	<boost/date_time/posix_time/posix_time.hpp> // for world time
	#define BOOST_DATE_TIME_POSIX_TIME_POSIX_TIME
#endif

#ifndef BOOST_DATE_TIME_POSIX_TIME_POSIX_TIME_DURATION
	#include	<boost/date_time/posix_time/posix_time_duration.hpp> // for calculate diff
	#define BOOST_DATE_TIME_POSIX_TIME_POSIX_TIME_DURATION
#endif

#ifndef ZEABUS_EXTENSION_CONVERT_TO_STRING
	#include	<zeabus_extension/convert_to_string.h> // for convert to string
	#define ZEABUS_EXTENSION_CONVERT_TO_STRING
#endif

#ifndef BOOST_THREAD_THREAD
	#include	<boost/thread/thread.hpp> //  for sleep
	#define BOOST_THREAD_THREAD
#endif

#ifndef nullptr
	#define nullptr 0
#endif

//////////////////////////////// Information Addition //////////////////////////////////////////
// type std::time_t accuracy is second
// protected are according inheritance to access. private can't do that
// this time have accuracy up to microsecond

// ------------------------------> for about name file <---------------------------------------
namespace world_time{

	class time{

		public:
			time( bool print_test = false); // for function init class
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
			time( bool print_test  = false);
			void reset_time();
			boost::posix_time::ptime now();
				
		protected:
			boost::posix_time::ptime time_01;
			boost::posix_time::time_duration temporary;
			void find_different();
			bool print_test;

	};

	class second : public time{
		public:
			second( bool print_test = false ) : time( print_test ){}
			long different( bool reset = true );
			void sleep( double data );
	};

	class millisecond : public time{
		public:
			millisecond( bool print_test = false ) : time( print_test ){}
			long different( bool reset = true );
			void sleep( double data );
	};

	class microsecond : public time{
		public:
			microsecond( bool print_test = false ) : time( print_test ){}
			long different( bool reset = true );
			void sleep( double data );
	};

	class nanosecond : public time{
		public:
			nanosecond( bool print_test  = false) : time( print_test ){}
			long different( bool reset = true );
			void sleep( double data );
	};

}

