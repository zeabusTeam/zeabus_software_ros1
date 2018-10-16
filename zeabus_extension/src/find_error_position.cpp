/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	File's name		: find_error_position.cpp
//
//	Create on		: Oct 15 , 2018
//	Author			: Supasan Komonlit
//
//	Main purpose	: detail of find_error_position.h
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _zeabus_extension_find_error_position_h__
	#include	<zeabus_extension/find_error_position.h>
	#define _zeabus_extension_find_error_position_h__
#endif

namespace zeabus_extension{
namespace zeabus_control{

template<typename number_type> void convert_angular( number_type& point){
	for( ; point < 0 || point > 2*PI ; ){
		if( point < 0 ) point += 2*PI;
		else if( point > 2*PI ) point -= 2*PI;
		else{
			std::cout	<< "<--ZEABUS_EXTENSION--> Error in loop function"
						<< " convert_angular in file find_error_position.cpp\n";
			exit( 1 );
		}
	}
}

template<typename number_type> number_type find_distance( int8_t size
														, number_type* point_1
														, number_type* point_2 ){
	number_type result = 0;
	for( int8_t run = 0 ; run < size ; run++){
		result += pow( point_1[run] - point_2[run] , 2);
	}
	#ifdef CHECK_FIND_ERROR
		std::cout << std::dec << "sum result is " << result << "\n";
	#endif
	return sqrt( result );
}

template<typename number_type> void find_min_angular( number_type& start 
											, number_type& target , number_type& result){
	#ifdef CHECK_FIND_ERROR
		std::cout << "before convert start " << start << "\n";
	#endif
	convert_angular( start );
	#ifdef CHECK_FIND_ERROR
		std::cout << "after convert start " << start << "\n";
	#endif
	convert_angular( target );
	number_type result_01 = target - start;
	number_type result_02;
	if( result_01 < 0 )  result_02 = PI*2 + ( result_01 );
	else  result_02 = -1 * ( PI*2 - ( result_01 ) );
	#ifdef CHECK_FIND_ERROR
	std::cout	<< "fabs of result_01 " << result_01 
				<< " and fabs of result_02 " << result_02 << "\n";
	#endif
	if( fabs( result_01 ) < fabs( result_02 ) ) result = result_01;
	else result = result_02;
}
// this is find distance when roll and pitch = 0 radian
template<typename number_type> void find_error_position( number_type *point_1 
				, number_type *point_2 , number_type *point_result ){
	point_result[0] = point_2[0] - point_1[0];	
	point_result[1] = point_2[1] - point_1[1];	
	point_result[2] = point_2[2] - point_1[2];
	find_min_angular( point_1[3] , point_2[3] , point_result[3]);	
	find_min_angular( point_1[4] , point_2[4] , point_result[4]);	
	find_min_angular( point_1[5] , point_2[5] , point_result[5]);	
}

}
}
