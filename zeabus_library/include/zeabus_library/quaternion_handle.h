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

#include	<zeabus_library/matrix.h>

#include	<zeabus_library/euler.h>

#include	<zeabus_library/Point4.h>

#include	<zeabus_library/convert_rotation.h>

#define		PI 3.14159265

#ifndef _ZEABUS_LIBRARY_QUATERNION_HANDLE_H__
#define _ZEABUS_LIBRARY_QUATERNION_HANDLE_H__

namespace zeabus_library{

	struct QuaternionVariable{
		public:
			QuaternionVariable();

			boost::numeric::ublas::matrix< double >matrix;
			double* w ;
			double* x ; 
			double* y ;
			double* z ;

			void set_quaternion( double roll , double pitch , double yaw );
			void set_quaternion( boost::numeric::ublas::matrix< double > matrix );
			void set_quaternion( zeabus_library::Point4 data );
			void set_quaternion( double w , double x , double y ,double z );

		private:
			boost::numeric::ublas::matrix< double > temp_matrix;
			double cos_yaw;
			double sin_yaw;
			double cos_pitch;
			double sin_pitch;
			double cos_roll;
			double sin_roll;

	};

	class QuaternionHandle : public QuaternionVariable{
		
		public:
			QuaternionHandle();

			void get_RPY( double& roll , double& pitch , double& yaw );

			void get_matrix_transform( boost::numeric::ublas::matrix< double > result );

			void get_matrix_transform( boost::numeric::ublas::matrix< double > result 
									, double roll , double pitch , double yaw );

			void get_matrix_transform( boost::numeric::ublas::matrix< double > result 
									, boost::numeric::ublas::matrix< double > target );

			void get_matrix_transform( boost::numeric::ublas::matrix< double > result 
									, zeabus_library::Point4 target );

			void get_matrix_transform( boost::numeric::ublas::matrix< double > result 
									, zeabus_library::QuaternionVariable data );

		private:
			zeabus_library::QuaternionVariable temp_quaternion;
			zeabus_library::MatrixRotation matrix_handle;
				
	};

}

#endif
