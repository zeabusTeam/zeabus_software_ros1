/*
	File name			:	matrix.cpp
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 20
	Date last modified	:	2018 , ??? ??
	Purpose				:	Function handle for matrix support only 
							boost matrix double
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Production

	Namespace			:	zeabus_library
*/

#include	<zeabus_library/matrix.h>

namespace zeabus_library{

	namespace matrix{

		void print( std::string message , boost::numeric::ublas::matrix< double >& data ){
			printf("%s\n\t[\n" , message.c_str() );
			for( int run_row = 0 ; run_row < data.size1() ; run_row++ ){
				printf("\t");
				for( int run_column = 0 ; run_column < data.size2() ; run_column++ ){
					printf("%10.3lf" , data( run_row , run_column ) );
				}
				printf("\n");
			}
			printf("\t]\n");
		}

		size_t	product(	boost::numeric::ublas::matrix< double >& m_1 
							,	boost::numeric::ublas::matrix< double >& m_2
							,	boost::numeric::ublas::matrix< double >& result 
							,	bool init_matrix ){
			if( m_1.size2() != m_2.size1() ){
				zeabus_library::print_error(
					"zeabus_library::matrix::product wrong size of condition for production");
				return zeabus_library::ERROR_SIZE_MATRIX;
			}
			else if( result.size1() != m_1.size1() || result.size2() != m_2.size2() ){
				zeabus_library::print_error(
					"zeabus_library::matrix::product wrong size of matrix result");
				return zeabus_library::ERROR_SIZE_MATRIX;
			}		
			boost::numeric::ublas::axpy_prod( m_1 , m_2 , result , init_matrix );
			return zeabus_library::NO_ERROR;
		}
		

		double det( boost::numeric::ublas::matrix< double >& matrix ){
			return matrix( 0 , 0 ) * matrix( 1 , 1 ) - matrix( 0 , 1 ) * matrix( 1 , 0 );
		}	
	}

}
