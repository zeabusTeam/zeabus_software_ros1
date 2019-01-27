/*
	File name			:	service_control.cpp	
	Author				:	Supasan Komonlit
	Date created		:	2019 , JAN 29
	Date last modified	:	2019 , JAN 29
	Purpose				:	This for manage about all service to connect with control
							This file will connect many variable of control please enure to all
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	Production

	Namespace			:	zeabus_library/control
*/

#include	<zeabus_library/control/service_control.h>

namespace euler_ = zeabus_library::euler;

namespace zeabus_library{

namespace control{

	ServiceControl::ServiceControl( int* mode ){
		this->register_mode( mode );
		this->quaternion.set_quaternion( 0 , 0 , 0 );
	} 

	void ServiceControl::register_position( zeabus_library::Point3* current 
							, zeabus_library::Point3* target , zeabus_library::Point3* diff ){
		this->current_position = current;
		this->target_position = target;
		this->diff_position = diff;	
	}

	void ServiceControl::register_euler( double* current , double* target , double* diff ){
		this->current_euler = current;
		this->target_euler = target;
		this->diff_euler = diff;
	}

	void ServiceControl::register_ok_error( double* ok_error ){
		this->ok_error = ok_error;
	}

	void ServiceControl::register_quaternion( zeabus_library::Point4* current 
							, zeabus_library::Point4* target){
		this->current_quaternion = current;
		this->target_quaternion = target;
	}

	void ServiceControl::register_mode( int* mode ){
		this->mode = mode;
	}

	bool ServiceControl::callback_relative_position( zeabus_library::TwoPoint::Request& request
											,zeabus_library::TwoPoint::Response& response ){
		this->target_position->x = request.x * cos( this->target_euler[2] )
								+ request.y * cos( this->target_euler[2] + euler_::PI );
		this->target_position->y = request.x * sin( this->target_euler[2] )
								+ request.y * sin( this->target_euler[2] + euler_::PI );
		response.result = true;
		return true;	
	}
	
	bool ServiceControl::callback_mode_control( zeabus_library::OneInt::Request& request 
									,	zeabus_library::OneInt::Response& response ){
		*(this->mode) = request.data;
		request.result = true;
		return true;
	}

	bool ServiceControl::callback_fix_point( zeabus_library::StrPoint::Request& request 
									, zeabus_library::StrPoint::Response& response ){
		if( request.type == "z" ) this->target_position->z = request.data;
		else if( request.type == "yaw" ){
			this->quaternion.set_quaternion( 0 , 0 , request.data );
			this->target_quaternion->w = this->quaternion->w;
			this->target_quaternion->x = this->quaternion->x;
			this->target_quaternion->y = this->quaternion->y;
			this->target_quaternion->z = this->quaternion->z;
		}
		else if( request.type == "+yaw" ){
			this->quaternion.set_quaternion( 0 , 0 , request.data + target_euler[2] );
			this->target_quaternion->w = this->quaternion->w;
			this->target_quaternion->x = this->quaternion->x;
			this->target_quaternion->y = this->quaternion->y;
			this->target_quaternion->z = this->quaternion->z;
		}
		request.result = true;
		return true;
	}

	bool ServiceControl::callback_check_position( zeabus_library::StrPoint::Request& request 
									, zeabus_library::StrPoint::Response& response ){
		if( request.type == "z" ){
			if( fabs( diff_position.z ) <= request.data + this->ok_error[2] ) 
				response.result = true;
			else response.result = false;
		}
		else if( request.type == "xy" ){
			double diff = sqrt( pow( diff_position.x , 2 ) + pow( diff_position.y , 2 ) );
			double ok_diff = sqrt( pow( ok_error[0] , 2 ) + pow( ok_error[1] , 2 ) );
			if( fabs( diff ) <= request.data + ok_diff ) response.result = true;
			else response.result = false;
		}
		else if( request.type == "yaw" ){
			if( fabs( diff_euler[2] ) <= request.data + ok_error[5] ) response.result = true;
			else response.result = false;
		}
		else response.response = false;
		return true;
	}

	bool ServiceControl::callback_get_target( zeabus_library::GetTwoPoint::Request& request 
									, zeabus_library::GetTwoPoint::Response& response ){
		if( request.type == "xy" ){
			response.x = target_position.x;
			response.y = target_position.y;
		}
		else if( request.type == "z" ){
			response.x = target_position.z;
		}
		else if( request.type == "yaw" ){
			response.x = target_euler[2];
		}
		return true;	
	}


}

}

