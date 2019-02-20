/*
	File name			:	ServiceThreeOdometry.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2018 , FEB 13
	Date last modified	:	2018 , ??? ??
	Purpose				:	

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	production

	Namespace			:	zeabus_library::control
*/
//===============>

#include	<zeabus_library/control/ServiceThreeOdometry.h>

namespace zeabus_library{

namespace control{

//===============> SERVICE FUNCTION
	bool ServiceThreeOdometry::callback_get_state( 
			zeabus_library::ThreeOdometry::Request& request
			, zeabus_library::ThreeOdometry::Response& response ){
		if( request.type == "current") response.data = *( this->current_state );
		else response.data = *( this->target_state );
	}

//===============> BASIC FUNCTION OF CLASS

	ServiceThreeOdometry::ServiceThreeOdometry(){};

	void ServiceThreeOdometry::register_all_state( nav_msgs::Odometry* data_current
			, nav_msgs::Odometry* data_target ){ 
		this->register_current( data_current );
		this->register_target( data_target );
	}

	void ServiceThreeOdometry::register_current( nav_msgs::Odometry* data ){
		this->current_state = data;
	}

	void ServiceThreeOdometry::register_target( nav_msgs::Odometry* data ){
		this->target_state = data;
	}

}

}
