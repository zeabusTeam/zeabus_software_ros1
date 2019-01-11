/*
	File name			:	listen_IMUQuaternion.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2019 , JAN 08
	Date last modified	:	2018 , ??? ??
	Purpose				:	
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Product

	Namespace			:	zeabus_library/localize
*/

#include	<zeabus_library/localize/listen_IMUQuaternion.h>

namespace zeabus_library{

namespace localize{

	ListenIMUQuaternion::ListenIMUQuaternion( zeabus_library::rotation::Quaternion& quaternion 
									, boost::numeric::ublas::matrix< double >& angular_velocity 
									, zeabus_library::Point4& linear_acceleration ){
		this->quaternion = quaternion;
		this->angular_velocity = angular_velocity;
		this->linear_acceleration = linear_acceleration;
	}

	void ListenIMUQuaternion::register_quaternion( 
							zeabus_library::rotation::Quaternion& variable ){
		this->quaternion = variable;
	}

	void ListenIMUQuaternion::register_acceleration(
							boost::numeric:::ublas::matrix< double >& variable ){
		this->linear_acceleration = variable;
	}

	void ListenIMUQuaternion::register_gyrometer( zeabus_library::Point3& variable ){
		this->angular_velocity = variable;
	}

	void ListenIMUQuaternion::callback( const zeabus_library::IMUQuaternion& message ){
		this->quaternion.set_quaternion( message.quaternion );
		this->angular_velocity = message.angular_velocity;
		this->linear_acceleration = message.linear_acceleration;
	}


}

}
