/*
	File name			:	euler.cpp	
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 17
	Date last modified	:	2018 , ??? ??
	Purpose				:	source code of manage and doing about standard euler
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	product

	Namespace			:	zeabus_library
*/

#include	<zeabus_library/euler.h>

#define _DEBUG_ZEABUS_LIBRARY_EULER_

namespace zeabus_library{

namespace euler{

	void bound_domain( double& radian ){
		#ifdef _DEBUG_ZEABUS_LIBRARY_EULER_
			printf( "Input zeabus_library::euler::bound_domain  < %.8lf > : Result " , radian );
		#endif
		for( ; radian < -1*PI ; radian += 2*PI );
		for( ; radian > PI ; radian -= 2*PI);
		#ifdef _DEBUG_ZEABUS_LIBRARY_EULER_
			printf( " < %.8lf > \n" , radian );
		#endif
	}

	void bound_show( double& radian ){
		#ifdef _DEBUG_ZEABUS_LIBRARY_EULER_
			printf( "Input zeabus_library::euler::bound_show  < %.8lf > : Result " , radian );
		#endif
		for( ; radian < 0 ; radian += 2*PI );
		for( ; radian > 2*PI ; radian -= 2*PI );
		#ifdef _DEBUG_ZEABUS_LIBRARY_EULER_
			printf( " < %.8lf > \n" , radian );
		#endif
	}

	double radian_domain( double radian ){
		#ifdef _DEBUG_ZEABUS_LIBRARY_EULER_
			printf( "Input zeabus_library::euler::radian_domain  < %.8lf > : Result " , radian );
		#endif
		for( ; radian < 0 ; radian += 2*PI );
		for( ; radian > 2*PI ; radian -= 2*PI );
		#ifdef _DEBUG_ZEABUS_LIBRARY_EULER_
			printf( " < %.8lf > \n" , radian );
		#endif
		return radian;
	}

	double degree_domain( double degree ){
		#ifdef _DEBUG_ZEABUS_LIBRARY_EULER_
			printf( "Input zeabus_library::euler::degree_domain  < %.8lf > : Result " , degree );
		#endif
		double radian = degree * PI / 180 ;
		bound_domain( radian );
		#ifdef _DEBUG_ZEABUS_LIBRARY_EULER_
			printf( " < %.8lf > \n" , radian );
		#endif
	}

	void sin( double radian , double& result ){
		bound_domain( radian );
		result = sin( radian );
	}	

	void cos( double radian , double& result ){
		bound_domain( radian );
		result = cos( radian );
	}

	void tan( double radian , double& result ){
		bound_domain( radian );
		result = tan( radian );
	}

	double sin( double radian ){
		bound_domain( radian );
		return sin( radian );
	}
	
	double cos( double radian ){
		bound_domain( radian );
		return cos( radian );
	}

	double tan( double radian ){
		bound_domain( radian );
		return tan( radian );
	}

}

}
