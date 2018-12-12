/*
	File name			:	quaternion_handle.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 05
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

#include	<zeabus_library/Point4.h>

#define		PI 3.14159265

#ifndef _ZEABUS_LIBRARY_QUAternion_handle__
#define _ZEABUS_LIBRARY_QUAternion_handle__

namespace zeabus_library{

	struct QuaternionVariable{
		public:
			QuaternionVariable();

			double &w;
			double &x;
			double &y;
			double &z;
			boost::numeric::ublas::matrix< double > matrix;

			void set_quaternion( double roll , double pitch , double yaw );
			void set_quaternion( boost::numeric::ublas::matrix< double > matrix );
			void set_quaternion( zeabus_library::Point4 data );
			void set_quaternion( double w , double x , double y ,double z );

	}

	class QuaternionHandle : public QuaternionVariable{
		
		public:
			QuaternionHandle();

			void get_RPY( double* roll , double* pitch , double* yaw );

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
			boost::numeric::ublas::matrix< double > temp_roll;
			boost::numeric::ublas::matrix< double > temp_pitch;
			boost::numeric::ublas::matrix< double > temp_yaw; 
			boost::numeric::ublas::matrix< double > temp_quaternion;
				
	};

}

#endif
