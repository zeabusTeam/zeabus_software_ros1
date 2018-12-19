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

	}

}
