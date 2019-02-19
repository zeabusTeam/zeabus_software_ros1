/*
	File name			:	service_two_bool.h		
	Author				:	Supasan Komonlit
	Date created		:	2019 , FEB 19
	Date last modified	:	2019 , ??? ??
	Purpose				:	
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Production

	Namespace			:	zeabus_library/service
*/
//===============>

#include	<stdio.h>

#include	<iostream>

#include	<zeabus_library/TwoBool.h>

#ifndef _ZEABUS_LIBRARY_SERVICE_TWO_BOOL_H_
#define _ZEABUS_LIBRARY_SERVICE_TWO_BOOL_H_

namespace zeabus_library{

namespace service{

	class ServiceTwoBool{

		public:
			ServiceTwoBool( bool* data = 0);
			
			void register_bool( bool* data);

			bool callback( zeabus_library::TwoBool::Request& request 
					, zeabus_library::TwoBool::Response& response );

		private:
			bool* data;

	};

}

}

#endif
