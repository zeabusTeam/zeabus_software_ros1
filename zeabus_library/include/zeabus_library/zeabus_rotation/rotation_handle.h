/*
	File name			:	rotation_handle.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 23
	Date last modified	:	2018 , ??? ??
	Purpose				:	This is header of code for quaternion and matrix rotation

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	test

	Namespace			:	zeabus_library
*/

#include	<stdio.h>

#include	<zeabus_library/zeabus_rotation/quaternion_handle.h>

#include	<zeabus_library/matrix.h>

#include	<zeabus_library/vector.h>

#include	<zeabus_library/error_code.h>

#ifndef _ZEABUS_LIBRARY_ZEABUS_ROTATION_ROTATION_HANDLE_H__
#define _ZEABUS_LIBRARY_ZEABUS_ROTATION_ROTATION_HANDLE_H__

namespace zeabus_library{

namespace zeabus_rotation{

	class RotationHandle : public QuaternionHandle{
	
		public:
			RotationHandle();
	
			size_t start_rotation( boost::numeric::ublas::matrix< double >& value 
							, boost::numeric::ublas::matrix< double >& result );

		private:
			boost::numeric::ublas::matrix< double > temporary_matrix;
			double diff_euler[3];	 

	} 

}

}

#endif
