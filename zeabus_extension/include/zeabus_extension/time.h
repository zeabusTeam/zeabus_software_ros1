#include	<iostream>
#include	<ctime>
#include	<boost/date_time/gregorian/gregorian.hpp>
#include	<boost/date_time/posix_time/posix_time.hpp>
#include	<zeabus_serial/convert_to_string.h>

// ------------------------------> for about name file <---------------------------------------
namespace world_time{
	class time{

		public:
			time( bool print_test ); // for function init class
			time();
			std::string local_time(); // return string for local time (computer time)
			std::string universal_time(); // return string for UTC time

		private:
			boost::posix_time::ptime time_data; // for collect data when init class
			bool print_test;
	};
}

// ---------------------------> use in code for calculate <------------------------------------
namespace local_time{
	class time{
		
		public:
			time( bool print_test ); // for function init class
			time();
			double diff_time(); // for calculate different time of two point
			void reset_time();
			
		private:
			bool print_test;
			time_t time_01;
			time_t time_02;

	};
}
