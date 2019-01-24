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

namespace zeabus_library{

	LinearEquation::LinearEquation( double x0 , double y0 , double x1 , double y1 ){
		this->set_point( x0 , y0 , x1 , y1 );
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
	}

	double LinearEquation::distance_point( double x , double y ){
		return zeabus_library::abs( this->A * x + this->B * y + this->C ) 
					/ ( sqrt( pow(this->A , 2) + pow(this->B , 2) ) );
	}
}
