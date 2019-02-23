/*
	File name			:	listen_basic.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , JAN 19
	Date last modified	:	2018 , ??? ??
	Purpose				:	For manage function in listen callback subscribe and access variable
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Product

	Namespace			:	zeabus_library
*/

#include	<stdio.h>

#include	<iostream>

#ifndef _ZEABUS_LIBRARY_LISTEN_BASIC_H__
#define _ZEABUS_LIBRARY_LISTEN_BASIC_H__

namespace zeabus_library{

	class ListenBasic{

		public:

			ListenBasic( int number = 20 );

			void set_limit_count( int number );			

			int count;

		protected:
			void set_count();

			int limit_count;
			

	};

}

#endif
