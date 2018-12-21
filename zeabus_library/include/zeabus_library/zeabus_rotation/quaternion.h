/*
	File name			:	quaternion.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 20
	Date last modified	:	2018 , ??? ??
	Purpose				:	Quaternion structur in form w x y z or matrix of boost

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Production

	Namespace			:	zeabus_library
*/

#include	<stdio.h>

#include	<boost/numeric/ublas/matrix.hpp>

#include	<exception>

#include	<math.h>

#include	<zeabus_library/vector.h>

#include	<zeabus_library/euler.h>

#include	<zeabus_library/Point4.h>

#include	<zeabus_library/error_code.h>

#ifndef PI
	#define		PI 3.14159265
#endif

#ifndef _ZEABUS_LIBRARY_QUATERNION_H__
#define _ZEABUS_LIBRARY_QUATERNION_H__

namespace zeabus_library{

namespace zeabus_rotation{

	struct Quaternion{
		public:
			Quaternion();

			boost::numeric::ublas::matrix< double >matrix;
			double* w ;
			double* x ; 
			double* y ;
			double* z ;

			void normalization();

			void get_RPY( double& roll , double& pitch , double& yaw );

			boost::numeric::ublas::matrix< double > inverse();
			size_t inverse( boost::numeric::ublas::matrix< double >& matrix_result );

			void set_quaternion( double roll , double pitch , double yaw );
			void set_quaternion( boost::numeric::ublas::matrix< double > matrix );
			void set_quaternion( zeabus_library::Point4 data );
			void set_quaternion( double w , double x , double y ,double z );

			void update_inverse();
			boost::numeric::ublas::matrix< double > inverse_matrix;
	

		private:
			double cos_yaw;
			double sin_yaw;
			double cos_pitch;
			double sin_pitch;
			double cos_roll;
			double sin_roll;
			size_t updated;

	};

}

}

#endif
