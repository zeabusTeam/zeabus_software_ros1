/*
	File name			:	pid.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2019 , JAN 25
	Date last modified	:	2018 , ??? ??
	Purpose				:	
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.1.0
	status				:	Product

	Namespace			:	zeabus_library/control
*/

#include	<zeabus_library/control/pid.h>

namespace zeabus_library{

namespace control{

	PID::PID( double p , double i , double d ){
		this->set_constant( p , i , d );
		this->offset_i( 0 );
		this->limit_i( 1 );
		this->set_period( 0.001 );
		this->reset_value();
	}

	PID::~PID(){}

	void PID::set_constant( double p , double i , double d ){
		this->p = p ;
		this->i = i ;
		this->d = d ;
		this->reset_value();
	}

	void PID::reset_value(){
		this->sum = 0;
		this->previous_error = 0;
		this->first_time = true;
	}

	void PID::set_period( double time ){
		this->period = time;
	}

	double PID::calculate( double error ){
		double answer = 0 ;
		this->calculate( error , answer );
		return answer;
	}

	void PID::offset_i( double offset ){
		this->offset = offset;
	}

	void PID::limit_i( double limit ){
		this->i_limit = limit;
	}

	void PID::set_max_limit( double limit ){
		this->max_limit = limit;
	}

	void PID::set_min_limit( double limit ){
		this->min_limit = limit;
	}

	void PID::set_limit( double min , double max ){
		this->min_limit = min;
		this->max_limit = max;
	}

	void PID::calculate( double error , double& result ){
		if( this->first_time ){
			this->sum = this->offset;
			this->previous_error = error;
			result = this->sum;
			this->first_time = false;
		}
		else{
			this->sum += ( ( error * this->i ) );
					+ ( error - this->previous_error) / this->period * this->d;

			if( zeabus_library::abs( this->sum ) > this->i_limit )
				this->sum = copysign( this->i_limit , this->sum );
			result = this->sum + error*this->p; 

			if( result > this->max_limit ) result = max_limit;
			else if( result < this->min_limit ) result = min_limit;

			this->previous_error = error;
		}
	}
}

}
