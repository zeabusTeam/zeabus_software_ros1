/*
	File name			:	listen_basic.cpp	
	Author				:	Supasan Komonlit
	Date created		:	2018 , JAN 19
	Date last modified	:	2018 , ??? ??
	Purpose				:	source code of manage and doing about standard euler
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	product

	Namespace			:	zeabus_library
*/

#include	<zeabus_library/listen_basic.h>

namespace zeabus_library{

	ListenBasic::ListenBasic( int number ){
		this->set_limit_count( number );
	}

	void ListenBasic::set_limit_count( int number ){
		this->limit_count = number;
	}

	void ListenBasic::set_count(){
		this->count = this->limit_count;
	}

}
