/*
	File name			:	capture_time.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 11
	Date last modified	:	2018 , ??? ??
	Purpose				:	For count_time
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Maintain

	Namespace			:	zeabus_library
*/

#include	<stdio.h>

#include	<boost/date_time/posix_time/posix_time.hpp>

#ifndef _ZEABUS_LIBRARY_CAPTURE_TIME_H__
#define _ZEABUS_LIBRARY_CAPTURE_TIME_H__

namespace zeabus_library{

	class CaptureTime{

		public:
			CaptureTime();
			~CaptureTime();
			void start();
			void capture( double& result , bool reset );
			double capture( bool reset = false );

		protected:
			void capture_time();

		private:
			boost::posix_time::ptime init_time;
			boost::posix_time::ptime current_time;
			boost::posix_time::time_duration duration_time;

	};

}

#endif
