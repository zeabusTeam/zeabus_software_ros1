/*
	File name			:	convert_rotation.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 05
	Date last modified	:	2018 , ??? ??
	Purpose				:	This is header of code for use matrix trasform by orientation

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

#define		PI 3.14159265


#ifndef _ZEABUS_LIBRARY_CONVERT_ROTATION_H__
#define _ZEABUS_LIBRARY_CONVERT_ROTATION_H__

namespace zeabus_library{

	class MatrixRotation{

		public:
			MatrixRotation();

			void roll_rotation( double roll 
									, boost::numeric::ublas::matrix< double >& result );
			void pitch_rotation( double pitch 
									, boost::numeric::ublas::matrix< double >& result );
			void yaw_rotation( double yaw 
									, boost::numeric::ublas::matrix< double >& result );

			void all_rotation( double roll , double pitch , double yaw 
									, boost::numeric::ublas::matrix< double >& result );

			void print_individual_matrix( std::string message 
											, boost::numeric::ublas::matrix< double>& data);
			void print_all_temp();

		protected:
			void check_dimension( boost::numeric::ublas::matrix< double >& result );

		private:
			boost::numeric::ublas::matrix< double > temp_yaw_rotation;
			boost::numeric::ublas::matrix< double > temp_roll_rotation;
			boost::numeric::ublas::matrix< double > temp_pitch_rotation;
			boost::numeric::ublas::matrix< double > temp_result;

	};

}

#endif
