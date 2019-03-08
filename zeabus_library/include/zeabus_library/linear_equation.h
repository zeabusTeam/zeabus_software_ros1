/*
	File name			:	linear_equation.h		
	Author				:	Supasan Komonlit
	Date created		:	2019 , JAN 24
	Date last modified	:	2019 , FEB 14
	Purpose				:	This file use for quation Ax + By + C = 0
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.3.1
	status				:	Using & Maintainance

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

		// Use cramers rule to solve equation and equation is -mx + y = b term Ax + By = D
		public:
			double A;
			double D;
			double A_inverse; //  A_inverse is A_2
			double D_2;

			LinearEquation( double x0 = 0 , double y0 = 0 , double x1 = 0 , double y1 = 0 );
		
			void register_point( double* point_x , double* point_y );
	
			void set_point( double x0 , double y0 , double x1 , double y1 );
			void set_point0( double x , double y );
			void set_point1( double x , double y );

			void update();
			void cut_point( double x , double y , double& cut_x , double& cut_y );
			void cut_point( double x , double y , double& cut_x , double& cut_y 
					, double& diff_x , double& diff_y );

			void find_point_x( double& x , double y );
			void find_point_y( double x , double& y ); 

			void print_equation();

		protected:
			double x0;
			double y0;
			double x1;
			double y1;
	
			double save_y;
			double save_x;

			int type; // 0 is normal 1 is pararell with x 2 is pararell with y
		private:
			double* point_x;
			double* point_y;
	
			double zero;

	};

}

#endif
