/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File's name		: log_data.h
//
//	Last Update		: Aug 17 , 2018
//	Author			: Supasan Komonlit
//
//	Main purpose	: for creat and save data to log is ROS
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef ROS_PACKAGE
	#include	<ros/package.h>
	#define ROS_PACKAGE
#endif

#ifdef IOSTREAM
	#include	<iostream>
	#define IOSTREAM
#endif

#ifndef CSTDLIB
	#include	<cstdlib>
	#define CSTDLIB
#endif

namespace ros{

	class find_path{

		public:
			find_path( std::string package_name , std::string path , std::string name_file );

		protected:
			std::string last_path;
		
	};

	class dynamic_reconfigure : public find_path{



	};
}

namespace log_data{

	class log{
		
	};
}
