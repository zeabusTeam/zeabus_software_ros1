/*
	File name			:	linear_equation.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , JAN 24
	Date last modified	:	2018 , ??? ??
	Purpose				:	This file use for quation Ax + By + C = 0
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Production

	Namespace			:	zeabus_library
*/

#include	<stdio.h>

#include	<zeabus_library/general.h>

#include	<zeabus_library/matrix.h>

#include	<math.h>

#include	<iostream>

#ifndef _ZEABUS_LIBRARY_LINEAR_EQUATION_H__
#define _ZEABUS_LIBRARY_LINEAR_EQUATION_H__

namespace zeabus_library{

	class LinearEquation{

		public:
			double A;
			double B;
			double C;
			double M;

			LinearEquation( double x0 = 0 , double y0 = 0 , double x1 = 0 , double y1 = 0 );
			
			void set_point( double x0 , double y0 , double x1 , double y1 );
			void set_point0( double x , double y );
			void set_point1( double x , double y );

			void update();
			double distance_point( double x , double y );
			void distance_split( double x , double y , double& ans_x , double& ans_y); 

		protected:
			double x0;
			double y0;
			double x1;
			double y1;
			double root_AB;

		private:
			boost::numeric::ublas::matrix< double > temp_matrix;

	};

}

#endif
