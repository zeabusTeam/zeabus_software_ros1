/*
	File name			:	listen_twist.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2019 , JAN 20
	Date last modified	:	2019 , JAN 27
	Purpose				:	
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.3
	status				:	USING & MAINTAINANCE

	Namespace			:	zeabus_library/control
*/

#include	<zeabus_library/control/listen_twist.h>

#define _DEBUG_RECEIVE_

namespace zeabus_library{

namespace control{

	ListenTwist::ListenTwist( int* count ){
		this->set_count = count;
		this->set_constant( 10 );
	}

	void ListenTwist::set_constant( int number ){
		this->constant = number;
	}

	void ListenTwist::register_linear( zeabus_library::Point3* variable ){
		this->linear = variable;
	}

	void ListenTwist::register_angular( zeabus_library::Point3* variable ){
		this->angular = variable;
	}

	void ListenTwist::callback( const zeabus_library::Twist& message ){
		*( this->linear ) = message.linear;
		*( this->angular ) = message.angular;
		this->check_zero_linear();
		this->check_zero_angular();
	}

	void ListenTwist::callback_linear( const zeabus_library::Twist& message ){
		*( this->linear ) = message.linear;
		this->check_zero_linear();
	}

	void ListenTwist::callback_angular( const zeabus_library::Twist& message ){
		*( this->angular ) = message.angular;
		this->check_zero_angular();
	}

	void ListenTwist::check_zero_linear(){
		if( (this->linear->x) > 10 ){
			this->set_count[0] = this->constant;
			(this->linear->x) = 0;
		}
		else if( this->linear->x < 0.001 && this->linear->x > -0.001 ){
			this->set_count[0] = 0;
			( this->linear->x ) = 0;
		}
		else{
			this->set_count[0] = this->constant;
		}

		if( (this->linear->y) > 10 ){
			this->set_count[1] = this->constant;
			(this->linear->y) = 0;
		}
		else if( this->linear->y < 0.001 &&  this->linear->y  > -0.001 ){
			this->set_count[1] = 0;
			( this->linear->y ) = 0;
		}
		else{
			this->set_count[1] = this->constant;
		}

		if( (this->linear->z) > 10 ){
			this->set_count[2] = this->constant;
			(this->linear->z) = 0;
		}
		else if(  this->linear->z < 0.001 && this->linear->z > -0.001   ){
			this->set_count[2] = 0;
			( this->linear->z ) = 0;
		}
		else{
			this->set_count[2] = this->constant;
		}
		#ifdef _DEBUG_RECEIVE_
			printf("ListenTwist::check_zero_linear finish check\n");
		#endif
	}

	void ListenTwist::check_zero_angular(){
		if( (this->angular->x) > 10 ){
			this->set_count[3] = this->constant;
			(this->angular->x) = 0;
		}
		else if( this->angular->x < 0.001 && this->angular->x  > -0.001 ){
			this->set_count[3] = 0;
			( this->angular->x ) = 0;
		}
		else{
			this->set_count[3] = this->constant;
		}

		if( (this->angular->y) > 10 ){
			this->set_count[4] = this->constant;
			(this->angular->y) = 0;
		}
		else if( this->angular->y  < 0.001 && this->angular->y > -0.001 ){
			this->set_count[4] = 0;
			( this->angular->y ) = 0;
		}
		else{
			this->set_count[4] = this->constant;
		}

		if( (this->angular->z) > 10 ){
			this->set_count[5] = this->constant;
			(this->angular->z) = 0;
		}
		else if(  this->angular->z < 0.001 && this->angular->z > -0.001){
			this->set_count[5] = 0;
			( this->angular->z ) = 0;
		}
		else{
			this->set_count[5] = this->constant;
		}
		#ifdef _DEBUG_RECEIVE_
			printf("ListenTwist::check_zero_angular finish check\n");
		#endif
	}

}

}
