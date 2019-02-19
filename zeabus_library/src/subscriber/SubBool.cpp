/*
	File name			:	SubBool.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2018 , FEB 18
	Date last modified	:	2018 , ??? ??
	Purpose				:	

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Using & Maintain
							
	Namespace			:	zeabus_library/subscriber
*/
//====================>

#include	<zeabus_library/subscriber/SubBool.h>

namespace zeabus_library{

namespace subscriber{

	SubBool::SubBool( std_msgs::Bool* data ){
		this->register_data( data );
	}

	void SubBool::register_data( std_msgs::Bool* data ){
		this->data = data;
	}

	void SubBool::callback( const std_msgs::Bool& message ){
		this->data->data = message.data;
	}

	void SubBool::callback_ttl( const std_msgs::Bool& message ){
		this->data->data = message.data;
		*(this->ttl) = this->constant_ttl;
	}

	void SubBool::register_ttl( int* ttl , int number){
		this->ttl = ttl;
		this->set_ttl( number );
	}

	void SubBool::set_ttl( int number ){
		this->constant_ttl = number;
	}

}

}
