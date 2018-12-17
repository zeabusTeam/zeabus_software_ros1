/*
	File name			:	euler.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 15
	Date last modified	:	2018 , ??? ??
	Purpose				:	This is header of code euler handle for manage bound value of euler

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	production

	Namespace			:	zeabus_library::euler
*/

#include	<stdio.h>

#include	<math.h>

#include	<exception>

#ifndef PI
	#define		PI 3.14159265
#endif

#ifndef _ZEABUS_LIBRARY_EULER_H__
#define _ZEABUS_LIBRARY_EULER_H__

namespace zeabus_library{

	namespace euler{

		// function for convert and bound 
		void bound_domain( double& radian );
		void bound_show( double& radian );

		// function for return value radian in bound for sin cos tan
		double radian_domain( double radian );
		double degree_domain( double degree );

		// function standard math in cpp and limit bound value of domain
		void sin( double radian , double& result );
		void cos( double radian , double& result );	
		void tan( double radian , double& result );
		double tan( double radian );
		double sin( double radian );
		double cos( double radian );

	}

}

#endif
