/*
	File name			:	convert_Type2.cpp	
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 07
	Date last modified	:	2018 , DEC 07
	Purpose				:	For convert array or value to zeabus_library/Type2
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Maintain

	Namespace			:	zeabus_library
*/

#include	<zeabus_library/convert_Type2.h>

namespace zeabus_library{

	void array_to_Type2( double* set_value , zeabus_library::Type2& result , int offset ){
		array_to_Point3( set_value , result.linear , offset );
		array_to_Point3( set_value , result.angular , offset + 3 );
	}

}

