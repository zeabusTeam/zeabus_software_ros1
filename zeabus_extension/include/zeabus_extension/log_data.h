/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File's name		: log_data.h
//
//	Last Update		: Aug 21 , 2018
//	Author			: Supasan Komonlit
//
//	Main purpose	: for creat and save data to log is ROS
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ROS_PACKAGE
	#include	<ros/package.h>
	#define ROS_PACKAGE
#endif

#ifndef IOSTREAM
	#include	<iostream>
	#define IOSTREAM
#endif

#ifndef CSTDLIB
	#include	<cstdlib>
	#define CSTDLIB
#endif

#ifndef ZEABUS_EXTENSION_TIME
	#include	<zeabus_extension/time.h>
	#define ZEABUS_EXTENSION_TIME
#endif

namespace ros{

	class find_path{

		public:
			find_path( std::string package_name , std::string path , std::string name_file );

		protected:
			set_path( std::string package_name , std::string path , std::string name_file);
			std::string last_path;
		
	};

	class dynamic_reconfigure : public find_path{

		public:
			dynamic_reconfigure( std::string package_name 
								, std::string path 
								, std::string name_file);
			set_parameter( std::string node_name );
			load();
			save();

	};
}

namespace log_data{

	class log : public ros::find_path , {

		public:	
			log( std::string package_name , std::string path 
				, std::string name_file = "" , bool anonymous = true);
			new_file( bool anonymous )
			write( std::string message );
			clear_screen();

		private: 
			count_time::time time = new count_time::time( true );

	};
}
