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
#define _DEBUG_RECIEVE_DATA_
#define _DEBUG_CALCULATE_ACCELERATION_
#define _DUBUG_CODE_

namespace zeabus_sensor{

	ListenIMUData::ListenIMUData( double roll , double pitch , double yaw , double gravity ){
		this->matrix_imu_to_robot.resize( 3 , 3 ); // set size of matrix
		this->matrix_imu_to_world.resize( 3 , 3 ); // set size of matrix
		this->matrix_robot_to_imu.resize( 3 , 3 ); // set size of matrix
		this->matrix_world_to_imu.resize( 3 , 3 ); // set size of matrix
		// find matrix to convert imu to robot so this is will have only one value	
		this->matrix_handle.all_rotation(	roll	, pitch		, yaw 
											, matrix_imu_to_robot );

		// set size matrix for result when want to get value
		this->result_euler.resize( 1 , 3 );
		this->result_gyro.resize( 1 , 3 );
		this->result_acceleration.resize( 1 , 3 );	

		// set size matrix for receive when want to get value
		this->receive_euler.resize( 1 , 3 );
		this->receive_gyro.resize( 1 , 3 );
		this->receive_acceleration.resize( 1 , 3 );	

		// set size matrix for collecting specific value
		this->offset_euler.resize( 1 , 3 );
		this->offset_gravity.resize( 1 , 3 );
		this->temporary_matrix.resize( 1 , 3 );
		this->set_orientation( roll , pitch , yaw );
		this->set_gravity( gravity );
	}
			
	void ListenIMUData::callback( const zeabus_library::IMUData& message ){
		zeabus_library::Point3_to_matrix( message.euler , this->receive_euler );
		zeabus_library::Point3_to_matrix( message.angular_velocity , this->receive_gyro );
		zeabus_library::Point3_to_matrix( message.linear_acceleration 
										, this->receive_acceleration );
		#ifdef _DEBUG_RECIEVE_DATA_
			this->matrix_handle.print_individual_matrix( "Receive_euler" , this->receive_euler );
			this->matrix_handle.print_individual_matrix( "Receive_gyro" , this->receive_gyro );
			this->matrix_handle.print_individual_matrix( "Receive_acceleration" 
															, this->receive_acceleration );
		#endif
		// step for delete gravity acceleration
		// find gravity in robot frame
		this->matrix_handle.all_rotation( this->receive_euler( 0 , 0 ) 
										, this->receive_euler( 0 , 1 )
										, this->receive_euler( 0 , 2 )
										, this->matrix_world_to_imu );
		this->temporary_matrix = boost::numeric::ublas::prod( this->offset_gravity 
															, this->matrix_world_to_imu );
		this->result_acceleration( 0 , 0 ) = this->receive_acceleration( 0 , 0 ) - 
												this->temporary_matrix( 0 , 0 ); 
		this->result_acceleration( 0 , 1 ) = this->receive_acceleration( 0 , 1 ) - 
												this->temporary_matrix( 0 , 1 ); 
		this->result_acceleration( 0 , 2 ) = this->receive_acceleration( 0 , 2 ) + 
												this->temporary_matrix( 0 , 2 ); 
		
		#ifdef _DEBUG_CALCULATE_ACCELERATION_
			this->matrix_handle.print_individual_matrix( "Matrix_world_to_imu"
															, this->matrix_world_to_imu );
			this->matrix_handle.print_individual_matrix( "Product offset and convert"
															, this->temporary_matrix );
			this->matrix_handle.print_individual_matrix( "Result_acceleration"
															, this->result_acceleration );
		#endif
	}

	void ListenIMUData::set_orientation( double roll , double pitch , double yaw ){
		this->offset_euler( 0 , 0 ) = roll ; 
		this->offset_euler( 0 , 1 ) = pitch;
		this->offset_euler( 0 , 2 ) = yaw;
	}

	void ListenIMUData::set_gravity( double gravity ){
		this->offset_gravity( 0 , 0 ) = 0 ;
		this->offset_gravity( 0 , 1 ) = 0 ;
		this->offset_gravity( 0 , 2 ) = gravity ;
	}

	void ListenIMUData::get_result( zeabus_library::IMUData& message ){
		split_matrix_to_IMUData( this->result_euler , this->result_gyro
								,this->result_acceleration , message );		
	}

}
