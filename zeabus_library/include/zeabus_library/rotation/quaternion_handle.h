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

	Namespace			:	zeabus_library/rotation
*/

#include	<stdio.h>

#include	<boost/numeric/ublas/matrix.hpp>

#include	<exception>

#include	<math.h>

#include	<zeabus_library/rotation/quaternion.h>

#include	<zeabus_library/vector.h>

#include	<zeabus_library/euler.h>

#include	<zeabus_library/error_code.h>

#include	<zeabus_library/Point4.h>

#ifndef PI
	#define	PI 3.14159265
#endif

#ifndef EPSILON
	#define	EPSILON 1.0e-7
#endif

// reference data information about quaternion to use rotation
//		http://www.chrobotics.com/library/understanding-quaternions

#ifndef _ZEABUS_LIBRARY_ROTATION_QUATERNION_HANDLE_H__
#define _ZEABUS_LIBRARY_ROTATION_QUATERNION_HANDLE_H__

namespace zeabus_library{

namespace rotation{

	class QuaternionHandle : public Quaternion{
		
		public:
			QuaternionHandle();

			size_t matrix_rotation( boost::numeric::ublas::matrix< double >& result 
									, bool inverse = false );

			// please warning about to set what frame is start what frame is target
			//		example value from IMU inertial frame is start and robot is target 

			void set_start_frame( double roll , double pitch , double yaw );
			void set_start_frame( zeabus_library::rotation::Quaternion quaternion );
			void set_start_frame( zeabus_library::Point4 data );

			void set_target_frame( double roll , double pitch , double yaw );
			void set_target_frame( zeabus_library::rotation::Quaternion quaternion );
			void set_target_frame( zeabus_library::Point4 data );
			
			// please call this function before get using rotation value 
			// but above required only you want current value from set up new quaternion
			void update_rotation();

			void start_rotation_target();
			void target_rotation_start();

			// In normal situation we don't suggest to this variable it for test only
			double diff_euler[3];

		protected:
			zeabus_library::rotation::Quaternion start_frame;
			zeabus_library::rotation::Quaternion target_frame;
			double start_euler[3];
			double target_euler[3];

	};

}

}

#endif
