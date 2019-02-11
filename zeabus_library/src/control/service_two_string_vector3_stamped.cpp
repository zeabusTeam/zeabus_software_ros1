/*
	File name			:	service_two_string_vector3_stamped.cpp		
	Author				:	Supasan Komonlit
	Date created		:	2018 , FEB 05
	Date last modified	:	2018 , ??? ??
	Purpose				:	

	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	1.0.0
	status				:	production

	Namespace			:	zeabus_library::control
*/
//===============>

#include	<zeabus_library/control/service_two_string_vector3_stamped.h>

namespace zeabus_library{

namespace control{

//===============> SERVICE CALLBACK PART
	void ServiceTwoStringVector3Stamped::callback_reset_velocity(
			zeabus_library::TwoStringVector3Stamped::Request& request,
			zeabus_library::TwoStringVector3Stamped::Response& response ){
		if( request.type == "xy"){
			this->fix_velocity[0] = false;
			this->fix_velocity[1] = false;
		}
		else if(request.type == "x" ){
			this->fix_velocity[0] = false;
		}
		else if(request.type == "y" ){
			this->fix_velocity[1] = false;
		}
		else if(request.type == "z" ){
			this->fix_velocity[2] = false;
		}
		else if( request.type == "yaw" ){
			this->fix_velocity[5] = false;

		}
		else if( request.type == "all" ){
			for( int run = 0 ; run < 6 ; run++ ) this->fix_velocity[run] = false;
		}
		response.result = true; 
	}

	void ServiceTwoStringVector3Stamped::callback_check_position(
			zeabus_library::TwoStringVector3Stamped::Request& request,
			zeabus_library::TwoStringVector3Stamped::Response& response ){
		response.result = false;
		if( request.type == "xy" ){
			this->temp_vector3 = current_quaternion->rotation( request.adding );
			if( ( fabs( this->current_state->pose.pose.position.x - 
					this->target_state->pose.pose.position.x) < this->temp_vector3.x ) && 
					(fabs( this->current_state->pose.pose.position.y -
							this->target_state->pose.pose.position.y ) < this->temp_vector3.y)){
				response.result = true;
			}
		}
		else if( request.type == "z" ){
			if(  fabs( this->current_state->pose.pose.position.z 
					- this->target_state->pose.pose.position.z) < request.adding.z ){
				response.result = true;
			}	
		}
		else if( request.type == "yaw" ){
			this->diff_quaternion->get_RPY( this->temp_double[0] , this->temp_double[1] 
					, this->temp_double[2] );
			if( fabs( this->temp_double[2] ) < request.adding.z ) response.result = true;
			else response.result = false;
		}
			
	}

	void ServiceTwoStringVector3Stamped::callback_reset_target(
			zeabus_library::TwoStringVector3Stamped::Request& request,
			zeabus_library::TwoStringVector3Stamped::Response& response ){
		if( request.type == "xy" ){
			this->target_state->pose.pose.position.x = this->current_state->pose.pose.position.x;
			this->target_state->pose.pose.position.y = this->current_state->pose.pose.position.y;
			response.result = true;
		}
		else if( request.type == "z"){
			this->target_state->pose.pose.position.z = this->current_state->pose.pose.position.z;
			response.result = true;
		}
		else if( request.type == "yaw" ){
			this->target_state->pose.pose.orientation = 
					this->current_state->pose.pose.orientation;
			response.result = true;
		}
		else response.result = false;
	}


//===============> SETUP CLASS PART

	ServiceTwoStringVector3Stamped::ServiceTwoStringVector3Stamped(){
		for( int run = 0 ; run < 3 ; run ++) this->temp_double[run] = 0; 
	}

	void ServiceTwoStringVector3Stamped::register_current( nav_msgs::Odometry* current ){
		this->current_state = current;
	}

	void ServiceTwoStringVector3Stamped::register_target( nav_msgs::Odometry* target ){
		this->target_state = target;
	}

	void ServiceTwoStringVector3Stamped::register_received( int* data ){
		this->received_reset = data;
	}

	void ServiceTwoStringVector3Stamped::register_velocity(bool*data_fix, double*data_velocity){
		this->fix_velocity = data_fix;
		this->value_velocity = data_velocity;
	}

	void ServiceTwoStringVector3Stamped::register_all_quaternion( 
			zeabus_library::tf_handle::TFQuaternion* data_current 
			, zeabus_library::tf_handle::TFQuaternion* data_target
			, zeabus_library::tf_handle::TFQuaternion* data_diff ){
		this->current_quaternion = data_current;
		this->target_quaternion = data_target;
		this->diff_quaternion = data_diff;
	}

}

}
