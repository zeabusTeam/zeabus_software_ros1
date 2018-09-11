/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File's name		: log_data.h
//
//	Last Update		: Sep 01 , 2018
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

#ifndef ZEABUS_EXTENSION_CONVERT_TO_STRING
	#include	<zeabus_extension/convert_to_string.h>
	#define ZEABUS_EXTENSION_CONVERT_TO_STRING
#endif

#define nullstring "~"

namespace zeabus_extension{
	namespace zeabus_ros{

		class find_path{

			public:
				find_path( std::string package_name 
						,  std::string path 
						,  std::string name_file );

			protected:
				void set_path( std::string package_name 
							,  std::string path 
							, std::string name_file);
				std::string last_path;
		
		};

		class dynamic_reconfigure : public find_path{

			public:
				dynamic_reconfigure( std::string package_name 
									, std::string path 
									, std::string name_file
									, std::string node_name);
				void set_parameter( std::string node_name );
				void load();
				void save();

			private:
				std::string node_name;

		};
	}
}

namespace zeabus_extension{
	namespace log_data{

		class log : public zeabus_extension::zeabus_ros::find_path {

			public:	
				log( std::string package_name 
						, std::string path 
						, std::string name_file = "~" 
						, bool anonymous = true);
				void new_file(std::string package_name = NULL
						, std::string path = NULL 
						, std::string name_file = NULL
						, bool anonymous = true);
				void write( std::string message , bool header = true , bool start = false);
				void clear_screen();

			private: 
				zeabus_extension::precise_time::millisecond time;
				zeabus_extension::world_time::time time_file;
				std::string name_file;
				std::string only_name;
				std::string package_name;
				std::string path;
		};
	}
}
