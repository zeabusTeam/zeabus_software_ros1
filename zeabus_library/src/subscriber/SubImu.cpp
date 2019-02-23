/*
	File name			:	SubImu.cpp		
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

#include	<zeabus_library/subscriber/SubImu.h>

namespace zeabus_library{

namespace subscriber{

	SubImu::SubImu( sensor_msgs::Imu* data ){
		this->register_data(data);
	}

	void SubImu::register_data( sensor_msgs::Imu* data ){
		this->data = data;
	}

	void SubImu::callback( const sensor_msgs::Imu& message ){
		*(this->data) = message;
	}

	void SubImu::callback_ttl( const sensor_msgs::Imu& message ){
		*(this->data) = message;
		*(this->ttl) = this->constant_ttl;
	}
	
	void SubImu::register_ttl( int* ttl , int number ){
		this->ttl = ttl;
		this->set_ttl( number );
	}

	void SubImu::set_ttl( int number ){
		this->constant_ttl = number;
	}


}

}
