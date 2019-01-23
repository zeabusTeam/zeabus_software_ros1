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

#define _DEBUG_CALL_BACK_QUATERNION_
#define _USE_DEBUG_SIZE_


namespace zeabus_library{

namespace localize{

	ListenIMUQuaternion::ListenIMUQuaternion( zeabus_library::Point4* quaternion 
									, zeabus_library::Point3* angular_velocity 
									, zeabus_library::Point3* linear_acceleration ) 
										: ListenBasic(){
		this->quaternion = quaternion;
		this->angular_velocity = angular_velocity;
		this->linear_acceleration = linear_acceleration;
	}

	void ListenIMUQuaternion::register_quaternion( 
							zeabus_library::Point4* variable ){
		this->quaternion = variable;
	}

	void ListenIMUQuaternion::register_gyrometer(
							zeabus_library::Point3* variable ){
		this->angular_velocity = variable;
	}

	void ListenIMUQuaternion::register_acceleration( zeabus_library::Point3* variable ){
		this->linear_acceleration = variable;
	}

	void ListenIMUQuaternion::callback( const zeabus_library::IMUQuaternion& message ){
		*(this->quaternion) = message.quaternion ;
		*(this->angular_velocity) = message.angular_velocity;
		*(this->linear_acceleration) = message.linear_acceleration;
		this->set_count();
	}

	void ListenIMUQuaternion::callback_quaternion( const zeabus_library::IMUQuaternion& message){
		#ifdef _DEBUG_CALL_BACK_QUATERNION_
			printf("ListenIMUQuaternion::callback_quaternion message receive\n");
			printf("\tw : %lf\n\tx : %lf\n\ty : %lf\n\tz : %lf\n" ,
					message.quaternion.w , message.quaternion.x , 
					message.quaternion.y , message.quaternion.z );
		#endif
		#ifdef _USE_DEBUG_SIZE_
			if( zeabus_library::convert::check_size( message.quaternion ) > 1.5 ) return;
		#endif
		*(this->quaternion) = message.quaternion;
		this->set_count();
	}
}

}
