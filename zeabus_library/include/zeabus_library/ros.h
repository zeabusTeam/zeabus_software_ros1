/*
	File name			:	ros.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , JAN 20
	Date last modified	:	2018 , ??? ??
	Purpose				:	This header library for managing log data will commerge ros and file
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.1.0
	status				:	maintainance

	Namespace			:	zeabus_library
*/

#include	<ros/ros.h>

#include	<stdio.h>

#include	<iostream>

#include	<ros/package.h>

#include	<zeabus_library/file.h> // include stuct directory

#include	<cstdlib>

#include	<zeabus_library/error_code.h>

#ifndef _ZEABUS_LIBRARY_ROS_H__
#define _ZEABUS_LIBRARY_ROS_H__


namespace zeabus_library{

	class DynParam{

		public:		
			DynParam();

			size_t set_package_file( std::string package );
			size_t set_directory_file( std::string directory );
			size_t set_name_file( std::string name );
			size_t set_node( std::string node_name );

			size_t dump();
			size_t load();

		protected:
			void update_directory();

		private:
			bool updated;

			std::string node_name;
			Directory location_file; // split to 3 part of string to commerge file

	};

}
#endif
