/*
	File name			:	SubTwistStamped.cpp		
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

#include	<zeabus_library/subscriber/SubTwistStamped.h>

namespace zeabus_library{

namespace subscriber{

	SubTwistStamped::SubTwistStamped( geometry_msgs::TwistStamped* data ){
		this->register_data(data);
	}

	void SubTwistStamped::register_data( geometry_msgs::TwistStamped* data ){
		this->data = data;
	}

	void SubTwistStamped::callback( const geometry_msgs::TwistStamped& message ){
		*(this->data) = message;
	}

	void SubTwistStamped::callback_ttl( const geometry_msgs::TwistStamped& message ){
		*(this->data) = message;
		if( fabs(message.twist.linear.x) > EPSILON ) this->ttl[0] = this->constant_ttl;
		else this->ttl[0] = 0;
		if( fabs(message.twist.linear.y) > EPSILON ) this->ttl[1] = this->constant_ttl;
		else this->ttl[1] = 0;
		if( fabs(message.twist.linear.z) > EPSILON ) this->ttl[2] = this->constant_ttl;
		else this->ttl[2] = 0;
		if( fabs(message.twist.angular.x) > EPSILON ) this->ttl[3] = this->constant_ttl;
		else this->ttl[3] = 0;
		if( fabs(message.twist.angular.y) > EPSILON ) this->ttl[4] = this->constant_ttl;
		else this->ttl[4] = 0;
		if( fabs(message.twist.angular.z) > EPSILON ) this->ttl[5] = this->constant_ttl;
		else this->ttl[5] = 0;
	}
	
	void SubTwistStamped::register_ttl( int* ttl , int number ){
		this->ttl = ttl;
		this->set_ttl( number );
	}

	void SubTwistStamped::set_ttl( int number ){
		this->constant_ttl = number;
	}


}

}
