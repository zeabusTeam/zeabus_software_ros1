/*
	File name			:	SubOdometry.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2018 , FEB 06
	Date last modified	:	2018 , ??? ??
	Purpose				:	

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Using & Maintain
							
	Namespace			:	zeabus_library/subscriber
*/
//====================>

#include	<zeabus_library/subscriber/SubOdometry.h>

namespace zeabus_library{

namespace subscriber{

	SubOdometry::SubOdometry( nav_msgs::Odometry* data ){
		this->register_data(data);
	}

	void SubOdometry::register_data( nav_msgs::Odometry* data ){
		this->data = data;
	}

	void SubOdometry::callback( const nav_msgs::Odometry& message ){
		*(this->data) = message;
	}

	void SubOdometry::callback_ttl( const nav_msgs::Odometry& message ){
		*(this->data) = message;
		*(this->ttl) = this->constant_ttl;
	}
	
	void SubOdometry::register_ttl( int* ttl , int number ){
		this->ttl = ttl;
		this->set_ttl( number );
	}

	void SubOdometry::set_ttl( int number ){
		this->constant_ttl = number;
	}


}

}
