/*
	File name			:	listen_DVL.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2019 , JAN 10
	Date last modified	:	2018 , ??? ??
	Purpose				:	
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Product

	Namespace			:	zeabus_library/localize
*/

#include	<zeabus_library/localize/listen_DVL.h>

#define _TEST_CALLBACK_DVL_

namespace zeabus_library{

namespace localize{

	ListenDVL::ListenDVL( zeabus_library::Point3* variable ){ 
		this->velocity = variable;	
	}

	size_t ListenDVL::register_velocity( zeabus_library::Point3* variable ){
		this->velocity = variable;
		return zeabus_library::NO_ERROR;
	}

	void ListenDVL::callback( const zeabus_library::Point3& message){
		#ifdef _TEST_CALLBACK_DVL_
			printf("Callback in ListenDVL\n\n");
		#endif
		*(this->velocity) = message;
	}


}

}
