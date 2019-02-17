/*
	File name			:	decision_part.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2019 , FEB 06
	Date last modified	:	2019 , FEB 09
	Purpose				:	

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Maintainance

	Namespace			:	None
*/
//====================>

#include	<stdio.h>

#include	<iostream>

#include	<math.h>

namespace zeabus_library{

namespace control{

	double velocity_xy( double error ){
		double temp = fabs( error );
		if( temp < 0.05 ) return 0;
		else if( temp < 0.3 ) return copysign( 0.1 , error );
		else if( temp < 0.6 ) return copysign( 0.2 , error );
		else if( temp < 1 ) return copysign( 0.5 , error );
		else return copysign( 1 , error );
	}

	double velocity_z( double error ){
		double temp = fabs( error );
		if( temp < 0.05 ) return 0;
		else if( temp < 0.25 ) return copysign( 0.1 , error );
		else if( temp < 0.5 ) return copysign(0.2 , error );
		else return copysign( 0.4 , error );
	}

	double velocity_yaw( double error ){
		double temp = fabs( error );
		if( temp < 0.1 ) return 0;
		else if( temp < 0.8 ) return copysign( 0.05 , error );
		else if( temp < 1 ) return copysign(0.1 , error );
		else return copysign( 0.3 , error );	
	}

}

}
