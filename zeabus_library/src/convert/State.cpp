/*
	File name			:	State.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 07
	Date last modified	:	2018 , DEC 27
	Purpose				:	For convert array or value to zeabus_library/State
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.1.0
	status				:	Maintain

	Namespace			:	zeabus_library/convert
*/

#include	<zeabus_library/convert/State.h>

namespace zeabus_library{
namespace convert{

	void array_to_State( double* set_value , zeabus_library::State& result , int offset){
		array_to_Type2( set_value , result.position , offset + 0 );
		array_to_Type2( set_value , result.velocity , offset + 6 );
	}

	void split_array_to_State( double* value_position , double* value_velocity
								, zeabus_library::State& result ){
		array_to_Type2( value_position , result.position , 0 );
		array_to_Type2( value_velocity , result.velocity , 0 );
	}

}
}
