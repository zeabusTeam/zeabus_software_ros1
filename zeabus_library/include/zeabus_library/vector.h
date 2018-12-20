/*
	File name			:	vector.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 20
	Date last modified	:	2018 , ??? ??
	Purpose				:	This header library for support using vector ( Quaternion )
							Support Boost matrix 
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Production

	Namespace			:	zeabus_library::vector
*/

#include	<stdio.h>

#include	<zeabus_library/error_code.h>

#include	<boost/numeric/ublas/matrix.hpp>

#ifndef _ZEABUS_LIBRARY_VECTOR_H__
#define _ZEABUS_LIBRARY_VECTOR_H__

namespace zeabus_library{

	namespace vector{

		// function production vector size 4x1 equation is v_1*v_2 = result 
		size_t multiplication( boost::numeric::ublas::matrix< double > v_1 
							, boost::numeric::ublas::matrix< double > v_2 
							, boost::numeric::ublas::matrix< double >& result );

	}

}

#endif
