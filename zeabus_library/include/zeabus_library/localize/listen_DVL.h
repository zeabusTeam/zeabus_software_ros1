/*
	File name			:	listen_DVL.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , JAN 10
	Date last modified	:	2018 , ??? ??
	Purpose				:	For receive message from dvl node
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Product

	Namespace			:	zeabus_library/localize
*/

#include	<stdio.h>

#include	<zeabus_library/matrix.h>

#include	<zeabus_library/error_code.h>

#include	<zeabus_library/Point3.h>

#ifndef _ZEABUS_LIBRARY_LOCALIZE_LISTEN_DVL_GEOMETRY_H__
#define _ZEABUS_LIBRARY_LOCALIZE_LISTEN_DVL_GEOMETRY_H__

namespace zeabus_library{

namespace localize{

	class ListenDVL{

		public:
			ListenDVL( zeabus_library::Point3* variable );
		
			size_t register_velocity( zeabus_library::Point3* variable );
			size_t 

			void callback( const zeabus_library::Point3& message );

			void set_count_receive( int number );

			int count; // for use to count receive

		private:
			int count_receive;
			zeabus_library::Point3* velocity;

	};

}

}

#endif
