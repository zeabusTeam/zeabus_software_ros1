/*
	File name			:	matrix.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 20
	Date last modified	:	2018 , ??? ??
	Purpose				:	This header library for support using matrix
							Support Boost matrix
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Production

	Namespace			:	zeabus_library::matrix
*/

#include	<stdio.h>

#include	<boost/numeric/ublas/matrix.hpp>

#include	<exception>

#ifndef _ZEABUS_LIBRARY_MATRIX_H__
#define _ZEABUS_LIBRARY_MATRIX_H__

namespace zeabus_library{

	namespace matrix{

		void print( std::string message , boost::numeric::ublas::matrix< double >& data );

	}

}

#endif
