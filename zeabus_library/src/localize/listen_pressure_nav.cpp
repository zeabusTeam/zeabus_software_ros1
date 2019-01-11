/*
	File name			:	listen_pressure_nav.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2019 , JAN 10
	Date last modified	:	2018 , ??? ??
	Purpose				:	
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Product

	Namespace			:	zeabus_library/localize
*/

#include	<zeabus_library/localize/listen_pressure_nav.h>

namespace zeabus_library{

namespace localize{

	ListenPressureNav::ListenPressureNav( double* variable ){
		this->depth = variable;
	}

	void ListenPressureNav::register_depth( double* variable){
		this->depth = variable;
	}

	void ListenPressureNav::callback( const nav_msgs::Odometry& message ){
		*(this->depth) = message.pose.pose.position.z;
	}

}

}
