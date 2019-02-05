/*
	File name			:	SubTwistWithCovarianceStamped.cpp		
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

#include	<zeabus_library/subscriber/SubTwistWithCovarianceStamped.h>

namespace zeabus_library{

namespace subscriber{

	SubTwistWithCovarianceStamped::SubTwistWithCovarianceStamped( 
			geometry_msgs::TwistWithCovarianceStamped* data ){
		this->register_data(data);
	}

	void SubTwistWithCovarianceStamped::register_data( 
			geometry_msgs::TwistWithCovarianceStamped* data ){
		this->data = data;
	}

	void SubTwistWithCovarianceStamped::callback( const 
			geometry_msgs::TwistWithCovarianceStamped& message ){
		*(this->data) = message;
	}

	void SubTwistWithCovarianceStamped::callback_ttl( const 
			geometry_msgs::TwistWithCovarianceStamped& message ){
		*(this->data) = message;
		*(this->ttl) = this->constant_ttl;
	}
	
	void SubTwistWithCovarianceStamped::register_ttl( int* ttl , int number ){
		this->ttl = ttl;
		this->set_ttl( number );
	}

	void SubTwistWithCovarianceStamped::set_ttl( int number ){
		this->constant_ttl = number;
	}


}

}
