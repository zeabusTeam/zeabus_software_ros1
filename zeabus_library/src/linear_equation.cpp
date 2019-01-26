/*
	File name			:	linear_equation.cpp	
	Author				:	Supasan Komonlit
	Date created		:	2018 , JAN 25
	Date last modified	:	2018 , ??? ??
	Purpose				:	This file use for equation Ax + By + C = 0
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	product

	Namespace			:	zeabus_library
*/

#include	<zeabus_library/linear_equation.h>

#define _DEBUG_ORDERED_

namespace zeabus_library{

	LinearEquation::LinearEquation( double x0 , double y0 , double x1 , double y1 ){
		this->set_point( x0 , y0 , x1 , y1 );
		temp_matrix.resize( 2 , 2 );
	}

	void LinearEquation::set_point( double x0 , double y0 , double x1 , double y1 ){
		this->set_point0( x0 , y0 );
		this->set_point1( x1 , y1 );
	}

	void LinearEquation::set_point0( double x , double y){
		this->x0 = x ;
		this->y0 = y ;
	}

	void LinearEquation::set_point1( double x , double y){
		this->x1 = x ;
		this->y1 = y ;
	}

	void LinearEquation::update(){
		this->A = this->y1 - this->y0;
		this->B = this->x0 - this->x1;
		this->C = (this->x1 * this->y0) - (this->y1 * this->x0 );
		this->M = (this->y1 - this->y0) / (this->x1 - this->x0 );
		this->root_AB = sqrt( pow(this->A , 2 ) + pow(this->B , 2 ) );
	}

	double LinearEquation::distance_point( double x , double y ){
		return zeabus_library::abs( this->A * x + this->B * y + this->C ) / this->root_AB;
	}

	void LinearEquation::distance_split( double x , double y , double& ans_x , double& ans_y){
		#ifdef _DEBUG_ORDERED_
			printf("BEGIN LinearEquation::distance_split\n");
		#endif
		double temp_M = -1.0 * this->M;
		double temp_C = y - ( temp_M * x );
		this->temp_matrix( 0 , 0 ) = temp_M; // -1.0 * this->M
		this->temp_matrix( 1 , 0 ) = this->M; // this->M
		this->temp_matrix( 0 , 1 ) = 1;
		this->temp_matrix( 1 , 1 ) = 1;
		// use Cramers Rule -> A X = D
		double det_A = zeabus_library::matrix::det( this->temp_matrix );
		this->temp_matrix( 0 , 0 ) = this->C;
		this->temp_matrix( 1 , 0 ) = temp_C;
		double cut_x = zeabus_library::matrix::det( this->temp_matrix ) / det_A;
		this->temp_matrix( 0 , 0 ) = temp_M; // -1.0 * this->M
		this->temp_matrix( 1 , 0 ) = this->M; // this->M
		this->temp_matrix( 0 , 1 ) = this->C;
		this->temp_matrix( 1 , 0 ) = temp_C;
		double cut_y = zeabus_library::matrix::det( this->temp_matrix ) / det_A;
		ans_x = cut_x - x ;
		ans_y = cut_y - y ;
		#ifdef _DEBUG_ORDERED_
			printf("END LinearEquation::distance_split\n");
		#endif
	}
}
