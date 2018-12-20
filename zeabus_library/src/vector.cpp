/*
	File name			:	vector.cpp
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 20
	Date last modified	:	2018 , ??? ??
	Purpose				:	Function handle for vector support only <quaternion>
							boost matrix double
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Production

	Namespace			:	zeabus_library
*/

#include	<zeabus_library/vector.h>

namespace zeabus_library{

	namespace vector{

		size_t multiplication( boost::numeric::ublas::matrix< double > v_1 
							, boost::numeric::ublas::matrix< double > v_2 
							, boost::numeric::ublas::matrix< double >& result ){
			if( v_1.size1() != 4 || v_1.size2() != 1){
				zeabus_library::print_error( 
					"zeabus_library::vector::multiplication v_1 wrong size matrix");
				return zeabus_library::ERROR_SIZE_MATRIX; 
			}
			else if( v_2.size1() !=4 || v_2.size2() != 1 ){
				zeabus_library::print_error( 
					"zeabus_library::vector::multiplication v_2 wrong size matrix");
				return zeabus_library::ERROR_SIZE_MATRIX; 
			}
			else if( result.size1() != 4 || result.size2() != 1 ){
				zeabus_library::print_error( 
					"zeabus_library::vector::multiplication result wrong size matrix");
				return zeabus_library::ERROR_SIZE_MATRIX; 
			}

			result( 0 , 0 ) =	v_1( 0 , 0 ) * v_2(  0 , 0 ) - v_1( 1 , 0 ) * v_2( 1 , 0 )
								- v_1( 2 , 0 ) * v_2( 2 , 0 ) - v_1( 3 , 0 ) * v_2( 3 , 0 );

			result( 1 , 0 ) =	v_1( 0 , 0 ) * v_2(  1 , 0 ) + v_1( 1 , 0 ) * v_2( 0 , 0 )
								+ v_1( 2 , 0 ) * v_2( 3 , 0 ) - v_1( 3 , 0 ) * v_2( 2 , 0 );

			result( 2 , 0 ) =	v_1( 0 , 0 ) * v_2(  2 , 0 ) - v_1( 1 , 0 ) * v_2( 3 , 0 )
								+ v_1( 3 , 0 ) * v_2( 0 , 0 ) + v_1( 3 , 0 ) * v_2( 1 , 0 );

			result( 3 , 0 ) =	v_1( 0 , 0 ) * v_2(  3 , 0 ) + v_1( 1 , 0 ) * v_2( 2 , 0 )
								- v_1( 2 , 0 ) * v_2( 1 , 0 ) + v_1( 3 , 0 ) * v_2( 0 , 0 );

			return zeabus_library::NO_ERROR;

		}

		size_t normalization( boost::numeric::ublas::matrix< double >& vector){
			if( vector.size1() != 4 || vector.size2() != 1 ){
				zeabus_library::print_error(
					"zeabus_library::vector::normalization vector wrong size matrix");
				return zeabus_library::ERROR_SIZE_MATRIX;
			}
			double all_degree_two = pow( vector( 0 , 0 ) , 2 )
									+ pow( vector( 1 , 0 ) , 2 )
									+ pow( vector( 2 , 0 ) , 2 )
									+ pow( vector( 3 , 0 ) , 2 );
			vector( 0 , 0 ) /= all_degree_two;
			vector( 1 , 0 ) /= all_degree_two;
			vector( 2 , 0 ) /= all_degree_two;
			vector( 3 , 0 ) /= all_degree_two;
			return zeabus_library::NO_ERROR;
		}

		void print( std::string message , boost::numeric::ublas::matrix< double >& vector ){
			zeabus_library::matrix::print( message , vector );
		}

		size_t conjugate( boost::numeric::ublas::matrix< double >& vector ){
			if( vector.size1() != 4 || vector.size2() != 1){
				zeabus_library::print_error(
					"zeabus_library::vector::conjugate vector wrong size matrix");
				return zeabus_library::ERROR_SIZE_MATRIX;
			}
			vector( 1 , 0 ) *= -1;
			vector( 2 , 0 ) *= -1;
			vector( 3 , 0 ) *= -1;
			return zeabus_library::NO_ERROR;
		}

		size_t conjugate( boost::numeric::ublas::matrix< double >& vector 
						, boost::numeric::ublas::matrix< double >& result ){
			if( vector.size1() != 4 || vector.size2() != 1 ){
				zeabus_library::print_error(
					"zeabus_library::vector::conjugate vector wrong size matrix");
				return zeabus_library::ERROR_SIZE_MATRIX;
			}
			else if( result.size1() != 4 || result.size2() != 1 ){
				zeabus_library::print_error(
					"zeabus_library::vector::conjugate result wrong size matrix");
				return zeabus_library::ERROR_SIZE_MATRIX;
			} 
			result( 0 , 0 ) = vector( 0 , 0 );
			result( 1 , 0 ) = vector( 1 , 0 ) * -1;	
			result( 2 , 0 ) = vector( 2 , 0 ) * -1;	
			result( 3 , 0 ) = vector( 3 , 0 ) * -1;	
			return zeabus_library::NO_ERROR;
		}

	}

}
