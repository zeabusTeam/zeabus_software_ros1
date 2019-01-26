/*
	File name			:	pid.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2019 , JAN 25
	Date last modified	:	2018 , ??? ??
	Purpose				:	
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
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
		this->limit = limit;
	}

	void PID::calculate( double error , double& result ){
		if( this->first_time ){
			this->sum = copysign( this->offset , error );
			this->previous_error = error;
			result = this->sum;
		}
		else{
			this->sum += ( error * this->i ) * this->period
					+ ( error - this->previous_error) / this->period;
			result = this->sum + error*this->p; 
			this->previous_error = error;
		}
	}
}

}
