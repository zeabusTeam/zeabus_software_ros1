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
							, boost::numeric::ublas::matrix< double > result ){
			if( v_1.size1() != 4 || v_1.size2() != 1){
				zeabus_library::print( 
					"zeabus_library::vector::multiplication v_1 wrong size matrix");
			}
			else if( v_2.size1() !=4 || v_2.size2() != 1 ){
				zeabus_library::print( 
					"zeabus_library::vector::multiplication v_2 wrong size matrix");
			}
			else if( result.size1() != 4 || result.size2() != 1 ){
				zeabus_library::print( 
					"zeabus_library::vector::multiplication result wrong size matrix");
			}
			return zeabus_library::ERROR_SIZE_MATRIX; 
		}			

	}

}
