/*
	File name			:	capture_time.cpp
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 11
	Date last modified	:	2018 , ??? ??
	Purpose				:	For count_time
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Maintain

	Namespace			:	zeabus_library
*/

#include	<zeabus_library/capture_time.h>

namespace zeabus_library{

	CaptureTime::CaptureTime(){}

	CaptureTime::~CaptureTime(){}

	void CaptureTime::start(){
		this->init_time = boost::posix_time::microsec_clock::local_time();
	}

	void CaptureTime::capture_time(){
		this->current_time = boost::posix_time::microsec_clock::local_time();
	}

	void CaptureTime::capture( double& result , bool reset ){
		this->capture_time();
		this->duration_time = this->current_time - this->init_time;
		result = double(duration_time.total_microseconds()) / 1000000;
		if( reset ) this->start();
	}

	double CaptureTime::capture( bool reset ){	
		this->capture_time();
		this->duration_time = this->current_time - this->init_time;
		if( reset ) this->start();
		return double(duration_time.total_microseconds()) / 1000000;
	}

}
