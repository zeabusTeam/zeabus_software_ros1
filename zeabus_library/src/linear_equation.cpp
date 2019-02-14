/*
	File name			:	linear_equation.cpp	
	Author				:	Supasan Komonlit
	Date created		:	2018 , JAN 25
	Date last modified	:	2018 , FEB 14
	Purpose				:	This file use for equation Ax + By + C = 0
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.3.1
	status				:	product

	Namespace			:	zeabus_library
*/

#include	<zeabus_library/linear_equation.h>

//#define _DEBUG_ORDERED_
//#define _DEBUG_DATA_

#ifdef _DEBUE_DATA_
	#include	<zeabus_library/text_color.h>
#endif

namespace zeabus_library{

	LinearEquation::LinearEquation( double x0 , double y0 , double x1 , double y1 ){
		this->zero = 0;
		this->register_point( &(this->zero) , &(this->zero) );
		this->set_point( x0 , y0 , x1 , y1 );
		this->update();
		this->D_2 = 0;
	}

	void LinearEquation::register_point( double* point_x , double* point_y ){
		this->point_x = point_x;
		this->point_y = point_y;
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
		double diff_of_y = this->y1 - this->y0;
		double diff_of_x = this->x1 - this->x0;
		if( fabs(diff_of_y) < 0.001 ){
			this->type = 1;
			this->save_y = this->y0;
		}
		else if( fabs(diff_of_x) < 0.001 ){
			this->type = 2;
			this->save_x = this->x0;
		}
		else{
			this->type = 0;
		}
		this->A = -1.0 * ( diff_of_y )/( diff_of_x );
		this->D = this->A * this->x1 + this->y1;
	}

	void LinearEquation::cut_point( double x , double y , double& cut_x , double& cut_y ){
		#ifdef _DEBUG_ORDERED_
			printf("BEGIN LinearEquation::distance_split and get cut point\n");
		#endif
		this->A_inverse = this->A * -1.0;
		this->D_2 = this->A_inverse * x + y;
		if( this->type == 0 ){
			cut_x = (this->D - D_2) / ( 2.0 * this->A );
			cut_y = (this->D + D_2) / this->A ;
		}
		else if( this->type == 1){
			cut_x = x;
			cut_y = this->save_y - y;
		}
		else{
			cut_x = this->save_x - x;
			cut_y = y;
		}
		#ifdef _DEBUG_DATA_
			zeabus_library::bold_red("LINEAR_EQUATION data\n");	
			printf("set1 A B D : %10.4lf%10.4lf%10.4lf\n" , this->A , 1.0 , this->D );
			printf("set2 A B D : %10.4lf%10.4lf%10.4lf\n" , A_inverse , 1.0 , D_2 );
			printf("RESULT     : %10.4lf%10.4lf\n" , cut_x , cut_y );
		#endif
		#ifdef _DEBUG_ORDERED_
			printf("END LinearEquation::distance_split\n");
		#endif
	}

	void LinearEquation::cut_point( double x , double y ,double& cut_x , double& cut_y 
			, double& diff_x , double& diff_y ){
		this->cut_point( x , y , cut_x , cut_y );
		diff_x = cut_x - *(this->point_x);
		diff_y = cut_y - *(this->point_y);
	}

	void LinearEquation::print_equation(){
		printf("set1 A B D : %10.4lf%10.4lf%10.4lf\n" , this->A , 1.0 , this->D );
		printf("set2 A B D : %10.4lf%10.4lf%10.4lf\n" , this->A_inverse , 1.0 , this->D_2 );
		printf("type save P: %10d%10.4lf%10.4lf\n" , this->type , this->save_x , this->save_y);
		
	}

}
