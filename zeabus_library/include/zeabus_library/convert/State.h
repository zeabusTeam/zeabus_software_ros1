/*
	File name			:	State.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 07
	Date last modified	:	2018 , DEC 26
	Purpose				:	For convert array or value to zeabus_library/State
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.1.0
	status				:	Maintain

	Namespace			:	zeabus_library
*/

#include	<stdio.h>

#include	<boost/numeric/ublas/matrix.hpp>

#include	<zeabus_library/convert/Type2.h>

#include	<zeabus_library/State.h>

#ifndef _ZEABUS_LIBRARY_CONVERT_STATE_H__
#define _ZEABUS_LIBRARY_CONVERT_STATE_H__

namespace zeabus_library{
namespace convert{

	void array_to_State( double* set_value , zeabus_library::State& result , int offset = 0 );

	void split_array_to_State( double* value_position , double* value_velocity
								, zeabus_library::State& result );

}
}

#endif
