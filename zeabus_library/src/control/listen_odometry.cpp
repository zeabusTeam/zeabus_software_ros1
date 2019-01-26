/*
	File name			:	listen_odometry.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2019 , JAN 20
	Date last modified	:	2018 , ??? ??
	Purpose				:	
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Product

	Namespace			:	zeabus_library/control
*/

#include	<zeabus_library/control/listen_odometry.h>

namespace zeabus_library{

namespace control{

	ListenOdometry::ListenOdometry(){
		this->first_time = true;
	}

	void ListenOdometry::register_linear_position( zeabus_library::Point3* variable ){
		this->linear_position = variable;
	}

	void ListenOdometry::register_quaternion( zeabus_library::Point4* variable ){
		this->quaternion = variable;
	}

	void ListenOdometry::register_target_position( zeabus_library::Point3* variable ){
		this->target_linear_position = variable;
	}

	void ListenOdometry::register_target_quaternion( zeabus_library::Point4* variable ){
		this->target_quaternion = variable;
	}

	void ListenOdometry::register_linear_velocity( zeabus_library::Point3* variable ){
		this->linear_velocity = variable;
	}

	void ListenOdometry::register_gyroscope( zeabus_library::Point3* variable ){
		this->gyroscope = variable;
	}

	void ListenOdometry::callback( const zeabus_library::Odometry& message ){
		*( this->linear_position ) = message.pose.position;
		*( this->linear_velocity ) = message.velocity.linear;
		*( this->quaternion ) = message.pose.quaternion;
		*( this->gyroscope ) = message.velocity.angular;
		if( first_time ){
			*( this->target_quaternion ) = message.pose.quaternion;
			*( this->target_linear_position ) = message.pose.position;
			first_time = false;
		}
	}

}

}
