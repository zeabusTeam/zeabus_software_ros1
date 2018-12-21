/*
	File name			:	quaternion_handle.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 12
	Date last modified	:	2018 , ??? ??
	Purpose				:	This is header of code for use matrix trasform by quaternion

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	test

	Namespace			:	zeabus_library
*/

#include	<stdio.h>

#include	<boost/numeric/ublas/matrix.hpp>

#include	<exception>

#include	<math.h>

#include	<zeabus_library/vector.h>

#include	<zeabus_library/error_code.h>

#include	<zeabus_library/euler.h>

#include	<zeabus_library/Point4.h>

#include	<zeabus_library/zeabus_rotation/quaternion.h>

#define		PI 3.14159265

// reference data information about quaternion to use rotation
//		http://www.chrobotics.com/library/understanding-quaternions

#ifndef _ZEABUS_LIBRARY_QUATERNION_HANDLE_H__
#define _ZEABUS_LIBRARY_QUATERNION_HANDLE_H__

namespace zeabus_library{

namespace zeabus_rotation{

	class QuaternionHandle : public Quaternion{
		
		public:
			QuaternionHandle();

			size_t matrix_rotation( boost::numeric::ublas::matrix< double >& result 
									, bool inverse = false );

			// please warning about to set what frame is start what frame is target
			//		example value from IMU inertial frame is start and robot is target 

			void set_start_frame( double roll , double pitch , double yaw );
			void set_start_frame( zeabus_library::zeabus_rotation::Quaternion quaternion );
			void set_start_frame( zeabus_library::Point4 data );

			void set_target_frame( double roll , double pitch , double yaw );
			void set_target_frame( zeabus_library::zeabus_rotation::Quaternion quaternion );
			void set_target_frame( zeabus_library::Point4 data );
			
			// please call this function before get using rotation value 
			// but above required only you want current value from set up new quaternion
			void update_rotation();

			void start_rotation_target();
			void target_rotation_start();

		protected:
			zeabus_library::zeabus_rotation::Quaternion start_frame;
			zeabus_library::zeabus_rotation::Quaternion target_frame;
			double start_euler[3];
			double target_euler[3];

	};

}

}

#endif
