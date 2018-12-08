/*
	File name			:	listen_IMUData.cpp
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 07
	Date last modified	:	2018 , ??? ??
	Purpose				:	This is source of file for listen zeabus_library/IMUData.h

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.0.1
	status				:	Product
							
	Namespace			:	zeabus_sensor
*/

#include	<zeabus_library/zeabus_sensor/listen_IMUData.h>

namespace zeabus_sensor{

	ListenIMUData::ListenIMUData( double roll , double pitch , double yaw , double gravity ){
		this->matrix_imu_to_robot.resize( 3 , 3 ); // set size of matrix
		this->matrix_imu_to_world.resize( 3 , 3 ); // set size of matrix
		// find matrix to convert imu to robot so this is will have only one value	
		this->matrix_handle.all_rotation(	roll	, pitch		, yaw 
											, matrix_imu_to_robot );

		// set size matrix for result when want to get value
		this->result_euler.resize( 1 , 3 );
		this->result_gyro.resize( 1 , 3 );
		this->result_acceleration.resize( 1 , 3 );	

		// srt size matrix for collecting specific value
		this->offset_euler.resize( 1 , 3 );
		this->offset_gravity.resize( 1 , 3 );
		this->set_orientation( roll , pitch , yaw );
		this->set_gravity( gravity );
	}
			
	void ListenIMUData::callback( const zeabus_library::IMUData& message ){
			
	}

	void set_orientation( double roll , double pitch , double yaw ){
		this->offset_euler( 0 , 0 ) = roll ; 
		this->offset_euler( 0 , 1 ) = pitch;
		this->offset_euler( 0 , 2 ) = yaw;
	}

	void set_gravity( double gravity ){
		this->offset_gravity( 0 , 0 ) = 0 ;
		this->offset_gravity( 0 , 1 ) = 0 ;
		this->offset_gravity( 0 , 2 ) = gravity ;
	}

	void ListenIMUData::get_result( zeabus_library::IMUData& message ){
		split_matrix_to_IMUData( this->result_euler , this->result_gyro
								,this->result_acceleration , message );		
	}

}
