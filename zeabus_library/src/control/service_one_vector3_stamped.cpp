/*
	File name			:	service_one_vector3_stamped.cpp		
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

#include	<zeabus_library/control/service_one_vecctor3_stamped.h>

namespace zeabus_library{

namespace control{

//===============> SERVICE FUNCTION
	void ServiceOneVector3Stamped::callback_relative_xy( 
			zeabus_library::OneVector3Stamped::Request& request
			, zeabus_library::OneVector3Stamped::Response& response ){
		this->temp_vector3 = this->target_quaternion->rotation( request.data );
		this->target_state.pose.pose.position.x += this->temp_vector3.x;	
		this->target_state.pose.pose.position.y += this->temp_vector3.y;
		response.result = true;	
	}

	void ServiceOneVector3Stamped::callback_relative_z(
			zeabus_library::OneVector3Stamped::Request& request
			, zeabus_library::OneVector3Stamped::Response& response ){
		this->target_state.z += request.data.z;
		response.result = true;
	}

	void ServiceOneVector3Stamped::callback_relative_yaw(
			zeabus_library::OneVector3Stamped::Request& request
			, zeabus_library::OneVector3Stamped::Response& response ){
		this->target_quaternion->get_RPY( this->temp_double[0] , this->temp_double[1] 
				, this->temp_double[2] );	
		this->temp_double[2]  += request.data.z;
		zeabus_library::tf_handle::edit_value( this->temp_double[2] );
		this->target_quaternion->setEulerZYX( this->temp_double[2] , this->temp_double[1]
				, this->temp_double[0] );
		this->target_state.pose.pose.orientation.x = this->target_quaternion.x();
		this->target_state.pose.pose.orientation.y = this->target_quaternion.y();
		this->target_state.pose.pose.orientation.w = this->target_quaternion.w();
		this->target_state.pose.pose.orientation.z = this->target_quaternion.z();
		response.result = true;	
	}

	void ServiceOneVector3Stamped::callback_fix_yaw( 
			zeabus_library::OneVector3Stamped::Request& request
			, zeabus_library::OneVector3Stamped::Response& response ){
		this->target_quaternion->get_RPY( this->temp_double[0] , this->temp_double[1] 
				, this->temp_double[2] );
		this->target_quaternion->setEulerZYX( response.data.z , this->temp_double[1]
				, this->temp_double[0] );
		this->target_state.pose.pose.orientation.x = this->target_quaternion.x();
		this->target_state.pose.pose.orientation.y = this->target_quaternion.y();
		this->target_state.pose.pose.orientation.w = this->target_quaternion.w();
		this->target_state.pose.pose.orientation.z = this->target_quaternion.z();
		response.result = true;
	}

	void ServiceOneVector3Stamped::callback_fix_z(
			zeabus_library::OneVector3Stamped::Request& request
			, zeabus_library::OneVector3Stamped::Response& response ){
		this->target_state.pose.pose.position.z = response.data.z;
		response.result = true;
	}

	void ServiceOneVector3Stamped::callback_velocity_xy( 
			zeabus_library::OneVector3Stamped::Request& request
			, zeabus_library::OneVector3Stamped::Response& response ){
		this->temp_vector3 = this->target_quaternion->rotation( request.data );
		this->equation->set_point( this->target_state.pose.pose.position.x
								, this->target_state.pose.pose.position.y )
		response.result = true;
	}

	void ServiceOneVector3Stamped::callback_velocity_z(
			zeabus_library::OneVector3Stamped::Request& request
			, zeabus_library::OneVector3Stamped::Response& response ){
		if(fabs( response.data.z < EPSILON ) ) this->fix_velocity[2] = 0;
		else{
			this->fix_velocity[2] = 1;
			this->value_velocity[2] = request.data.z;
		} 
		response.result = true;
	}

	void ServiceOneVector3Stamped::callback_velocity_yaw(
			zeabus_library::OneVector3Stamped::Request& request
			, zeabus_library::OneVector3Stamped::Response& response ){
		if(fabs( response.data.z < EPSILON ) ) this->fix_velocity[2] = 0;
		else{
			this->fix_velocity[5] = 1;
			this->value_velocity[5] = request.data.z;
		}
		response.result = true;
	}

//===============> BASIC FUNCTION OF CLASS

	ServiceOneVector3Stamped::ServiceOneVector3Stamped(){
		this->temp_double = new double[3] {0 , 0 , 0 };
	};

	void ServiceOneVector3Stamped::register_all_state( nav_msgs::Odometry* data_current
			, nav_msgs::Odometry* data_target , nav_msgs::Odometry* data_linear ){
		this->register_current( data_current );
		this->register_target( data_target );
		this->register_linear( data_linear );
	}

	void ServiceOneVector3Stamped::register_all_quaternion( 
			zeabus_library::tf_handle::TFQuaternion* data_current 
			, zeabus_library::tf_handle::TFQuaternion* data_target
			, zeabus_library::tf_handle::TFQuaternion* data_diff ){
		this->current_quaternion = data_current;
		this->target_quaternion = data_target;
		this->diff_quaternion = data_diff;
	}

	void ServiceOneVector3Stamped::register_current( nav_msgs::Odometry* data ){
		this->current_state = data;
	}

	void ServiceOneVector3Stamped::register_target( nav_msgs::Odometry* data ){
		this->target_state = data;
	}

	void ServiceOneVector3Stamped::register_linear( nav_msgs::Odometry* data ){
		this->linear_state = data;
	}

	void ServiceOneVector3Stamped::register_velocity( bool* data_fix , double* data_velocity ){
		this->fix_velocity = data_fix;
		this->value_velocity = data_velocity;
	}

	void ServiceOneVector3Stamped::register_equation( zeabus_library::LibearEquation* data ){
		this->equation = data;
	}

}

}
