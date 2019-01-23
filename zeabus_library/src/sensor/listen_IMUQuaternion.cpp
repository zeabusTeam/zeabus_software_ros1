/*
	File name			:	listen_IMUQuaternion.cpp
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 24
	Date last modified	:	2018 , DEC 27
	Purpose				:	This is source of file for listen zeabus_library/IMUQuaternion.h

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Product
							
	Namespace			:	zeabus_library/sensor
*/

#include	<zeabus_library/sensor/listen_IMUQuaternion.h>

#define _DEBUG_RECIEVE_DATA_
//#define _DEBUG_CALCULATE_ACCELERATION_
//#define _DEBUG_CALCULATE_QUATERNION_
#define _DEBUG_CALCULATE_GYROMETER_
#define _DEBUG_CODE_
#define _DEBUG_TIME_
#define _DEBUG_DECLARE_OBJECT_

#define _USE_DEBUG_SIZE_

namespace zeabus_library{

namespace sensor{

	ListenIMUQuaternion::ListenIMUQuaternion( double roll , double pitch 
												, double yaw , double gravity ){
		RH_world.set_start_frame( 0 , 0 , 0 );
		RH_robot.set_start_frame( 0 , 0 , 0 );
		RH_robot.set_target_frame( roll , pitch , yaw );
		this->diff_quaternion.set_quaternion( roll , pitch , yaw );
		this->result_acceleration.resize( 3 , 1 );
		this->return_acceleration.resize( 3 , 1 );
		this->receive_gyro.resize( 3 , 1 );
		this->result_gyro.resize( 3 , 1 );
		this->receive_quaternion.resize( 4 , 1 );
		this->result_quaternion.resize( 4 , 1 );
		this->receive_acceleration.resize( 3 , 1 );
		this->offset_gravity.resize( 3 , 1 );
		this->set_gravity( gravity );
	}

	void ListenIMUQuaternion::copy_value( zeabus_library::IMUQuaternion target ){
		this->receive_msg.quaternion = target.quaternion;
		this->receive_msg.angular_velocity = target.angular_velocity;
		this->receive_msg.linear_acceleration = target.linear_acceleration;
	}
			
	void ListenIMUQuaternion::callback( const zeabus_library::IMUQuaternion& message ){
		#ifdef _DEBUG_TIME_
			this->timer.start();
		#endif
		this->receive_msg = message;
		#ifdef _DEBUG_CODE_
			printf("Convert queaternion msg -> matrix\n");
		#endif
		zeabus_library::convert::Point4_to_matrix( this->receive_msg.quaternion 
											, this->receive_quaternion );
		#ifdef _DEBUG_CODE_
			printf("Convert angular velocity msg -> matrix\n");
		#endif
		zeabus_library::convert::Point3_to_matrix( this->receive_msg.angular_velocity 
											, this->receive_gyro );
		#ifdef _DEBUG_CODE_
			printf("Convert linear acceleration msg -> matrix\n");
		#endif
		zeabus_library::convert::Point3_to_matrix( this->receive_msg.linear_acceleration 
											, this->receive_acceleration );
		// ======================> part calculate euler <=======================================
		zeabus_library::vector::multiplication( this->diff_quaternion.matrix 
												, this->receive_quaternion 
												, this->result_quaternion );		
		#ifdef _DEBUG_CALCULATE_QUATERNION_
			zeabus_library::vector::print("receive quaternion" , this->receive_quaternion );
			zeabus_library::vector::print("diff quaternion" , this->diff_quaternion.matrix );
			zeabus_library::vector::print("result quaternion" , this->result_quaternion );
			this->temp_quaternion.set_quaternion( this->receive_quaternion );
			this->temp_quaternion.get_RPY( this->roll , this->pitch , this->yaw );
			printf(" receive roll : pitch : yaw === %8.4lf : %8.4lf : %8.4lf\n" 
										, this->roll , this->pitch , this->yaw );
			this->temp_quaternion.set_quaternion( this->diff_quaternion.matrix );
			this->temp_quaternion.get_RPY( this->roll , this->pitch , this->yaw );
			printf(" diff roll : pitch : yaw === %8.4lf : %8.4lf : %8.4lf\n" 
										, this->roll , this->pitch , this->yaw );
			this->temp_quaternion.set_quaternion( this->result_quaternion );
			this->temp_quaternion.get_RPY( this->roll , this->pitch , this->yaw );
			printf(" result roll : pitch : yaw === %8.4lf : %8.4lf : %8.4lf\n" 
										, this->roll , this->pitch , this->yaw );
		#endif
		
		// ======================> part calculate acceleration <================================
		RH_world.set_target_frame( message.quaternion );
		RH_world.target_rotation( this->receive_acceleration , this->result_acceleration);
		this->return_acceleration = this->result_acceleration + this->offset_gravity;
			
		#ifdef _DEBUG_CALCULATE_ACCELERATION_
			zeabus_library::vector::print("receive acceleration" , this->receive_acceleration );
			zeabus_library::vector::print("Result acceleration " , this->result_acceleration );
			zeabus_library::vector::print("Offset gravity" , this->offset_gravity );
			zeabus_library::vector::print("return acceleration" , this->return_acceleration );
		#endif	

		// ============================> part calculate gyrometer <=============================
		RH_robot.target_rotation( this->receive_gyro , this->result_gyro );
		#ifdef _DEBUG_CALCULATE_GYROMETER_
			zeabus_library::vector::print("receive gyrometer" , this->receive_gyro );
			zeabus_library::vector::print("result gyrometer" , this->result_gyro );
		#endif
		
		#ifdef _DEBUG_TIME_
			printf("Last line of callback listen_IMUData : %15.8f\n" , this->timer.capture() );
		#endif

	}

	void ListenIMUQuaternion::set_orientation( double roll , double pitch , double yaw ){
		RH_robot.set_start_frame( roll , pitch , yaw );
	}

	void ListenIMUQuaternion::set_gravity( double gravity ){
		this->offset_gravity( 0 , 0 ) = 0 ;
		this->offset_gravity( 1 , 0 ) = 0 ;
		this->offset_gravity( 2 , 0 ) = gravity ;
	}

	void ListenIMUQuaternion::get_result( zeabus_library::IMUQuaternion& message ){
		zeabus_library::convert::split_matrix_to_IMUQuaternion( this->result_quaternion 
								, this->result_gyro
								,this->return_acceleration , message );	
	}

}
}
