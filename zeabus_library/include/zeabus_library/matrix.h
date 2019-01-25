/*
	File name			:	matrix.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 20
	Date last modified	:	2018 , ??? ??
	Purpose				:	This header library for support using matrix
							Support Boost matrix
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.1.0
	status				:	Use & Maintainance

	Namespace			:	zeabus_library::matrix
*/

#include	<stdio.h>

#include	<boost/numeric/ublas/matrix.hpp>

#include	<boost/numeric/ublas/operation.hpp>

#include	<zeabus_library/error_code.h>

#include	<exception>

#ifndef _ZEABUS_LIBRARY_MATRIX_H__
#define _ZEABUS_LIBRARY_MATRIX_H__

namespace zeabus_library{

namespace matrix{

	void print( std::string message , boost::numeric::ublas::matrix< double >& data );

	size_t product( boost::numeric::ublas::matrix< double >& m_1
					, boost::numeric::ublas::matrix< double >& m_2 
					, boost::numeric::ublas::matrix< double >& result 
					, bool init_matrix = true );

	double det( boost::numeric::ublas::matrix< double >& matrix );
}

}

#endif
