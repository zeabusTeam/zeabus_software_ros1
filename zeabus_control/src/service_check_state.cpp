////////////////////////////////////// DETAIL ///////////////////////////////////////////////////
//
//	File		: service_check_state.cpp 
//	Purpose		: for sub service of control
//
//	Created by	: Supasan Komonlit
//	Created on	: 2018, Oct 22
//
//	namespace	: zeabus_control
//
///////////////////////////////////// END PART//////////////////////////////////////////////////


#include	<iostream>
#include	"main_service.pp"
//#define	_CHECK_ERROR_

#ifndef _service_check_state.cpp

namespace zeabus_control{
	class check_state_service : public main_service{
		public:
	}; 
}

#endif

#ifdef _CHECK_ERROR_
	#undef _CHECK_ERROR_
#endif
