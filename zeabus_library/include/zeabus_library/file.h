/*
	File name			:	file.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , JAN 20
	Date last modified	:	2018 , ??? ??
	Purpose				:	This header library for managing log data will commerge ros and file c
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Production

	Namespace			:	zeabus_library
*/

#include	<ros/ros.h>

#include	<stdio.h>

#include	<iostream>

#include	<ros/package.h>

#include	<zeabus_library/error_code.h>

#include	<zeabus_library/convert/general.h>

#include	<zeabus_library/convert/boost_time.h>

#include	<boost/date_time/posix_time/posix_time.hpp>

#ifndef _ZEABUS_LIBRARY_FILE_H__
#define _ZEABUS_LIBRARY_FILE_H__


namespace zeabus_library{

	struct Directory{
		std::string package;
		std::string subdirectory;
		std::string name_file;
		std::string result;
	};

	class File{

		public:		
			File( std::string package , std::string directory 
						, std::string name , bool time = true);
			~File();

			size_t write( std::string& message );
			size_t writeline( std::string message );

			size_t set_package_file( std::string package );
			size_t set_directory_file( std::string directory );
			size_t set_name_file( std::string name , bool time =  true );

			size_t open();
			size_t close();

			void update_time();
		
		protected:
			void update_directory();

		private:
			bool status_file; // false is close and true file are now open

			FILE *file; // file in c system will use for write or read
	
			Directory location_file; // split to 3 part of string to commerge file

			boost::posix_time::ptime time_ptime;
			std::string time;
			
	};

}
#endif
